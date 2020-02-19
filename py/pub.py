from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import time
import argparse
import json

def setup():
    host            = "a1mp26epubc42b-ats.iot.ca-central-1.amazonaws.com"
    rootCAPath      = "/home/mwen/cert/aws/root-CA.crt"
    certificatePath = "/home/mwen/cert/aws/MoreyLight.cert.pem"
    privateKeyPath  = "/home/mwen/cert/aws/MoreyLight.private.key"
    useWebsocket    = False
    clientId        = "server"
    port            = 8883

    # Init AWSIoTMQTTClient
    myAWSIoTMQTTClient = AWSIoTMQTTClient(clientId)
    myAWSIoTMQTTClient.configureEndpoint(host, port)
    myAWSIoTMQTTClient.configureCredentials(rootCAPath, privateKeyPath, certificatePath)

    # AWSIoTMQTTClient connection configuration
    myAWSIoTMQTTClient.configureAutoReconnectBackoffTime(1, 32, 20)
    myAWSIoTMQTTClient.configureOfflinePublishQueueing(-1)  # Infinite offline Publish queueing
    myAWSIoTMQTTClient.configureDrainingFrequency(2)  # Draining: 2 Hz
    myAWSIoTMQTTClient.configureConnectDisconnectTimeout(10)  # 10 sec
    myAWSIoTMQTTClient.configureMQTTOperationTimeout(5)  # 5 sec

    # Connect and subscribe to AWS IoT
    myAWSIoTMQTTClient.connect()

    return myAWSIoTMQTTClient


def publish(myAWSIoTMQTTClient):
    # Publish to the same topic in a loop forever
    topic   = "raspberryPi"
    message = {}
    message['command'] = "light"
    messageJson = json.dumps(message)
    myAWSIoTMQTTClient.publish(topic, messageJson, 1)

def disconnect(myAWSIoTMQTTClient):
    myAWSIoTMQTTClient.disconnect()

a = setup();
publish(a);
disconnect(a);

