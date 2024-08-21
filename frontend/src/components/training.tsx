import React, { useState, useEffect } from 'react';

const TrainSection: React.FC = () => {
  const [socket, setSocket] = useState<WebSocket | null>(null);
  const [isArduinoConnected, setIsArduinoConnected] = useState<boolean>(true);

  const alphabetAndNumbers = [...'ABCDEFGHIJKLMNOPQRSTUVWXYZ', ...'0123456789'];

  useEffect(() => {
    const ws = new WebSocket('ws://localhost:8080');
    setSocket(ws);

    ws.onopen = () => {
      console.log('Connected to backend');
    };

    ws.onmessage = (message) => {
      const data = JSON.parse(message.data);
      if (data.arduinoConnected) {
        setIsArduinoConnected(true);
      } else {
        setIsArduinoConnected(false);
      }
    };

    ws.onclose = () => {
      setIsArduinoConnected(false);
    };

    return () => {
      ws.close();
    };
  }, []);

  const handleClick = (value: string) => {
    if (socket && isArduinoConnected) {
      socket.send(JSON.stringify({ type: 'send-data', data: value }));
    }
  };

  return (
    <div className="p-6">
      <h1 className="text-xl font-bold mb-4">
        Arduino Connection Status: {isArduinoConnected ? 'Connected' : 'Disconnected'}
      </h1>
      {isArduinoConnected && (
        <div className="grid grid-cols-6 gap-2">
          {alphabetAndNumbers.map((value) => (
            <button
              key={value}
              className="bg-blue-500 text-white p-2 rounded hover:bg-blue-600"
              onClick={() => handleClick(value)}
            >
              {value}
            </button>
          ))}
        </div>
      )}
    </div>
  );
};

export default TrainSection;
