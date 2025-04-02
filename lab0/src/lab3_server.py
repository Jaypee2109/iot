from flask import Flask, request

app = Flask(__name__)


@app.route("/sensor")
def receive_sensor_data():
    # Grab the query parameters sent by the ESP32
    temperature = request.args.get("temperature")
    humidity = request.args.get("humidity")

    # Debug printing
    print(f"Received temperature = {temperature} C, humidity = {humidity} %")

    # Respond back to the client
    return f"Server received temperature = {temperature} C, humidity = {humidity} %"
