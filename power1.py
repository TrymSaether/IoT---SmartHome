import paho.mqtt.client as mqtt
import csv
import pandas as pd
import schedule
import time



def on_connect(client, userdata, flags, rc): #subskriber
    if (rc==0):
        print("successful connection")
        client.subscribe("homeHub/booking/kitchen")
        client.subscribe("homeHub/booking/bathroom1")
        client.subscribe("homeHub/booking/livingroom")
        client.subscribe("homeHub/room/ceilingspeed")
        client.subscribe("homeHub/room/heater")
        client.subscribe("homeHub/power/azimuth")
        client.subscribe("homeHub/power/sleep")
    else:
        print("connection fail") 

def on_message(client, userdata, msg): # for å mota meldinger
    msg.payload = ("{}".format(msg.payload.decode("utf-8"))) #oversetter meldinger
    new_power = pd.read_csv("power.csv") # leser csv fil
    
    if msg.topic == "homeHub/booking/kitchen":  # for motatt melding
        if(msg.payload == 'true'):
            power_consumption = 6.0 #strømforbruk i min
            power(msg.topic, power_consumption) #funksjon for lagring av strøm
        elif(msg.payload == 'false'):
            power_consumption = 0.5
            power(msg.topic, power_consumption)
            
    if msg.topic == "homeHub/booking/bathroom1":  
        if(msg.payload == 'available'):
            power_consumption = 0.5
            power(msg.topic, power_consumption)
        elif(msg.payload == "booked"):
            power_consumption = 6.2
            power(msg.topic, power_consumption)
            print(new_power)
            
    if msg.topic == "homeHub/booking/livingroom":  
        if(msg.payload == 'available'):
            power_consumption = 0.0
            power(msg.topic, power_consumption)
        elif(msg.payload == "booked"):
            power_consumption = 28
            power(msg.topic, power_consumption)
            
    if msg.topic == "homeHub/room/ceilingspeed":  
        if(msg.payload == ""):
            power_consumption = 0.4
            power(msg.topic, power_consumption)
        elif(msg.payload == ""):
            power_consumption = 0.0
            power(msg.topic, power_consumption)
            
    if msg.topic == "homeHub/room/light":  
        if(msg.payload == 'true'):
            power_consumption = 0.1
            power(msg.topic, power_consumption)
        elif(msg.payload == 'false'):
            power_consumption = 0.1
            power(msg.topic, power_consumption)
        elif(msg.payload == 'trueon'):
            power_consumption = 0.1
            power(msg.topic, power_consumption)
            
    if msg.topic == "homeHub/power/azimuth":  #usikker på hvordan den fungerer
        azimuthAdj = (int(msg.payload) - 20)
        if(azimuthAdj <= 180 or azimuthAdj >= 0):
            power_consumption = -10
            power(msg.topic, power_consumption)
    if msg.topic == "homeHub/power/consumption":  
        
        power_consumption = int(msg.payload)
        return power_consumption
    
def power(topic, usage): # lagring av forbruk som er akkurat nå
    new_power = pd.read_csv("power.csv") #leser csv fil
    new_power.at[0, topic] = usage #legger inn ny verdi i rett felt for rett topic
    time.sleep(1)
    new_power.to_csv("power.csv", index=False) # lagrer endringer
    print(new_power)
    

def timer(): # kjøres hvert min så forbruk på hver av enhetene må oppgis i kw min
    new_power = pd.read_csv("power.csv") #leser av csv fil
    new_power.at[0, "sum"] = (new_power.sum(axis=1)) # sumerer forbrukert 
    power_consumption = str(new_power.at[0, "sum"]) # lagrer sum for å sende til node red
    new_power.to_csv("power.csv", index=False) # lagrer endrin av sum
    print(new_power)
    client.publish("homeHub/power_consumption", str(power_consumption)) # seder forbruk til node red

def weekly_power_consumption(): # kjøres hver søndag kl 2359 laster opp ukentlig forbruk til node red 
    new_power = pd.read_csv("power.csv") # leser csv fil
    power_consumption_weekly = str(new_power.at[0, "sum"]) # lagrer sum
    client.publish("homeHub/power_consumption/weekly", str(power_consumption_weekly)) # laster opp ukentlig forbruk
    new_power.at[0, "sum"] = 0 # nullstiller forbruk
    new_power.to_csv("power.csv", index=False) # lagrer endring av forbruk




client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message


client.connect("192.168.1.11", 1883)
schedule.every(1).minutes.do(timer)
schedule.every().sunday.at("23:59").do(weekly_power_consumption)

while True:
    client.loop_start()
    schedule.run_pending()
    client.loop_stop()
    


