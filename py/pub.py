from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import time
import argparse
import json
import sqlite3

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

def subscribe_callback(client, userdata, message):
    payload_msg = message.payload.decode("utf-8")
    try:
        payload_json = json.loads(message.payload.decode("utf-8"))
        data_list    = payload_json["data"]

        tableName = "STATE"

        for element in data_list:
            e_id    = element["Id"]
            e_time  = element["Time"]
            e_state = 1 if element["State"] == "on" else 0

            conn = sqlite3.connect("/home/mwen/data/light.db")
            c    = conn.cursor()
            c.execute("CREATE TABLE IF NOT EXISTS {} (Id INTEGER PRIMARY KEY, date INTEGER, state INTEGER)".format(tableName))
            insertmsg = ("INSERT OR REPLACE INTO {} "
                    "(Id, date, state) VALUES ({}, {}, {})").format(tableName, e_id, e_time, e_state)
            c.execute(insertmsg)
            conn.commit()
            conn.close()
    except:
        print("error")
        write_error("error parse message")

def subscribe(myAWSIoTMQTTClient):
    try:
        myAWSIoTMQTTClient.subscribe('server', 1, subscribe_callback)
    except:
        write_error("error subscribe")
        disconnect(myAWSIoTMQTTClient)
        myAWSIoTMQTTClient = setup()
    return myAWSIoTMQTTClient

def publish(myAWSIoTMQTTClient, command):
    # Publish to the same topic in a loop forever
    try:
        topic   = "raspberryPi"
        message = {}
        message['command'] = command 
        messageJson = json.dumps(message)
        myAWSIoTMQTTClient.publish(topic, messageJson, 1)
    except:
        write_error("error publish")
        disconnect(myAWSIoTMQTTClient)
        myAWSIoTMQTTClient = setup()
        myAWSIoTMQTTClient = subscribe(myAWSIoTMQTTClient)
    return myAWSIoTMQTTClient

def disconnect(myAWSIoTMQTTClient):
    myAWSIoTMQTTClient.disconnect()

def write_error(message):
    errorFile = open("error.txt", "a");
    errorFile.write(message + "\n")
    errorFile.close()

def write_response(message):
    r = open("r.txt", "a");
    r.write(message + "\n")
    r.close()
