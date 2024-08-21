import WebSocket, { Server } from 'ws';
import {SerialPort} from 'serialport';

const wss = new Server({ port: 8080 });
let arduinoConnected = false;

// Example: Establish serial connection with Arduino
const port = new SerialPort({path: '/dev/tty.Bluetooth-Incoming-Port', baudRate: 9600 });

port.on('open', () => {
  console.log('Serial Port Opened');
  arduinoConnected = true;
  notifyClientsArduinoConnection();
});

port.on('data', (data: Buffer) => {
  console.log('Received from Arduino:', data.toString());
});

port.on('close', () => {
  console.log('Serial Port Closed');
  arduinoConnected = false;
  notifyClientsArduinoConnection();
});

const notifyClientsArduinoConnection = () => {
  wss.clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(JSON.stringify({ arduinoConnected }));
    }
  });
};

wss.on('connection', (ws: WebSocket) => {
  console.log('Client connected');

  // Send initial connection status to frontend
  ws.send(JSON.stringify({ arduinoConnected }));

  ws.on('message', (message: string) => {
    const { type, data } = JSON.parse(message);
    
    if (type === 'send-data') {
      console.log(`Received data to send to Arduino: ${data}`);
      
      // Send data to Arduino if connected
      if (arduinoConnected) {
        port.write(data);
      }
    }
  });

  ws.on('close', () => {
    console.log('Client disconnected');
  });
});
