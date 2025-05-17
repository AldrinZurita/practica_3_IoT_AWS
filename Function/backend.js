const Alexa = require('ask-sdk-core');
const AWS = require('aws-sdk');

// Configuración mejorada de AWS IoT
AWS.config.update({
    region: 'us-east-2' // Corregido a us-east-2 para que coincida con tu endpoint
});

const iotdata = new AWS.IotData({
    endpoint: 'a8zrduyiqifqz-ats.iot.us-east-2.amazonaws.com',
    region: 'us-east-2',
    maxRetries: 3,
    httpOptions: { timeout: 5000 }
});

const ShadowParams = {
    thingName: 'Panel_Objeto'
};

// ===== [ Funciones AWS IoT Mejoradas ] =====
const handleIoTError = (error) => {
    if (error.code === 'ForbiddenException') {
        console.error('❌ Error de permisos. Verifica IAM:', JSON.stringify({
            action: error.operation,
            resource: `arn:aws:iot:${error.region}:*:thing/${ShadowParams.thingName}`
        }, null, 2));
        throw new Error('Configuración de permisos incorrecta');
    }
    throw error;
};

const getShadowPromise = (params) => new Promise((resolve, reject) => {
    console.log("📡 Enviando solicitud GET a IoT Core...");
    iotdata.getThingShadow(params, (err, data) => {
        if (err) return reject(handleIoTError(err));
        try {
            const payload = JSON.parse(data.payload);
            console.log("✅ /get/accepted:", JSON.stringify({
                reported: payload.state?.reported,
                desired: payload.state?.desired
            }, null, 2));
            resolve(payload);
        } catch (parseError) {
            console.error('❌ Error parsing shadow:', parseError);
            reject(parseError);
        }
    });
});

const updateShadowPromise = (params) => new Promise((resolve, reject) => {
    console.log("🔄 Enviando actualización a IoT Core...");
    iotdata.updateThingShadow(params, (err, data) => {
        if (err) return reject(handleIoTError(err));
        try {
            const payload = JSON.parse(data.payload);
            console.log("✅ /update/accepted:", JSON.stringify(payload.state, null, 2));
            resolve(payload);
        } catch (parseError) {
            console.error('❌ Error parsing shadow update:', parseError);
            reject(parseError);
        }
    });
});

// ===== [ Handlers Corregidos ] =====

const LaunchRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'LaunchRequest';
    },
    handle(handlerInput) {
        console.log("🚀 LaunchRequest recibido");
        return handlerInput.responseBuilder
            .speak('Bienvenido al control de iluminación. Di "consultar estado" o "ajustar intensidad".')
            .reprompt('¿Qué deseas hacer?')
            .getResponse();
    }
};

const StateIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'StateIntent';
    },
    async handle(handlerInput) {
        console.log("🔍 StateIntent recibido");
        try {
            const shadowData = await getShadowPromise(ShadowParams);
            const reported = shadowData.state?.reported || {};
            const desired = shadowData.state?.desired || {};
            
            const currentIntensity = reported.lightLevel || 'desconocida';
            let speakOutput = `La intensidad actual es ${currentIntensity}.`;
            
            if (desired.lightLevel && desired.lightLevel !== reported.lightLevel) {
                speakOutput += ` Hay un cambio pendiente a ${desired.lightLevel}.`;
            }

            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt('¿Necesitas algo más?')
                .getResponse();
        } catch (error) {
            console.error('❌ Error en StateIntent:', error.message);
            return handlerInput.responseBuilder
                .speak('No pude consultar el estado. Por favor intenta más tarde.')
                .reprompt('¿Quieres intentarlo de nuevo?')
                .getResponse();
        }
    }
};
const DegreeIntensityIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'DegreeIntensityIntentHandler';
    },
    async handle(handlerInput) {
        console.log("📏 DegreeIntensityIntent recibido");

        try {
            const shadowData = await getShadowPromise(ShadowParams);
            const reported = shadowData.state?.reported || {};
            const intensity = reported.intensity;

            if (intensity === undefined || intensity === null) {
                return handlerInput.responseBuilder
                    .speak('La intensidad de luz no está disponible en este momento.')
                    .reprompt('¿Deseas intentar de nuevo?')
                    .getResponse();
            }

            return handlerInput.responseBuilder
                .speak(`La intensidad actual de la luz es de ${intensity} por ciento.`)
                .reprompt('¿Te gustaría hacer algo más?')
                .getResponse();
        } catch (error) {
            console.error('❌ Error en DegreeIntensityIntent:', error.message);
            return handlerInput.responseBuilder
                .speak('Ocurrió un error al obtener la intensidad. Intenta más tarde.')
                .reprompt('¿Quieres intentar otra acción?')
                .getResponse();
        }
    }
};

const AdjustServoIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AdjustServoIntentHandler';
    },
    async handle(handlerInput) {
        console.log("🔄 AdjustServoIntent recibido");
        
        try {
            // Preparar la actualización del shadow con "actualizar"
            const updateParams = {
                ...ShadowParams,
                payload: JSON.stringify({
                    state: {
                        desired: {
                            servoAngle: "actualizar"
                        }
                    }
                })
            };
            
            // Enviar la actualización
            await updateShadowPromise(updateParams);
            
            return handlerInput.responseBuilder
                .speak(`Se ha solicitado actualizar el servomotor.`)
                .reprompt('¿Necesitas otra acción con el servomotor?')
                .getResponse();
        } catch (error) {
            console.error('❌ Error en AdjustServoIntent:', error.message);
            return handlerInput.responseBuilder
                .speak('No pude solicitar la actualización del servomotor. Por favor intenta más tarde.')
                .reprompt('¿Quieres intentarlo de nuevo?')
                .getResponse();
        }
    }
};

const HelpIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.HelpIntent';
    },
    handle(handlerInput) {
        console.log("🆘 HelpIntent recibido");
        return handlerInput.responseBuilder
            .speak('Puedes decir: "¿Cuál es la intensidad?" o "Cambia la intensidad a medio".')
            .reprompt('¿En qué puedo ayudarte?')
            .getResponse();
    }
};

const CancelStopIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && (Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.CancelIntent'
            || Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.StopIntent');
    },
    handle(handlerInput) {
        console.log("🛑 Cancel/StopIntent recibido");
        return handlerInput.responseBuilder
            .speak('Hasta luego.')
            .getResponse();
    }
};

const SessionEndedRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'SessionEndedRequest';
    },
    handle(handlerInput) {
        console.log("🔚 Sesión terminada. Razón:", handlerInput.requestEnvelope.request.reason);
        return handlerInput.responseBuilder.getResponse();
    }
};

const ErrorHandler = {
    canHandle() {
        return true;
    },
    handle(handlerInput, error) {
        console.error('=== ERROR CRÍTICO ===');
        console.error('Request:', JSON.stringify(handlerInput.requestEnvelope.request, null, 2));
        console.error('Error stack:', error.stack);
        
        return handlerInput.responseBuilder
            .speak('Lo siento, hubo un error. Por favor inténtalo más tarde.')
            .getResponse();
    }
};

// ===== [ Configuración Final ] =====
exports.handler = Alexa.SkillBuilders.custom()
    .addRequestHandlers(
        LaunchRequestHandler,
        StateIntentHandler,
        DegreeIntensityIntentHandler, // Nombre corregido
        AdjustServoIntentHandler,
        HelpIntentHandler,
        CancelStopIntentHandler,
        SessionEndedRequestHandler
    )
    .addErrorHandlers(ErrorHandler)
    .withCustomUserAgent('iot-control/v2')
    .lambda();