import express from 'express';
import bodyParser from 'body-parser';
import mongoose, { ConnectOptions } from 'mongoose';

// Define the schema for the sensor data
interface SensorData {
  moisture: string;
  temperature: string;
}

// Define the schema for the sensor
interface Sensor {
  [key: string]: SensorData;
}

// Define the schema for the MongoDB document
interface SensorDocument extends mongoose.Document {
  sensor: string;
  data: Sensor;
}

// Create the MongoDB model
const SensorModel = mongoose.model<SensorDocument>(
  'Sensor',
  new mongoose.Schema({
    sensor: String,
    data: Object,
  })
);

// Connect to the MongoDB database
mongoose.connect('mongodb://mongodb:27017/sensors', {
  useNewUrlParser: true,
  useUnifiedTopology: true,
} as ConnectOptions);

// Create the Express app
const app = express();

// Use body-parser middleware to parse JSON request bodies
app.use(bodyParser.json());

// Define the route to receive sensor data
app.post('/send-data', async (req, res) => {
  try {
    // Save the sensor data to the database
    const { sensor, data } = req.body;
    await SensorModel.create({ sensor, data });

    // Send a response
    res.status(200).send('Sensor data received');
  } catch (err) {
    console.error(err);
    res.status(500).send('Error saving sensor data');
  }
});

// Define the route to display sensor data
app.get('/show-data', async (req, res) => {
  try {
    // Get all the sensor data from the database
    const sensorData = await SensorModel.find();

    // Display the sensor data on a webpage
    const html = `
      <html>
        <head>
          <title>Sensor Data</title>
        </head>
        <body>
          <h1>Sensor Data</h1>
          <table>
            <thead>
              <tr>
                <th>Sensor</th>
                <th>Moisture</th>
                <th>Temperature</th>
              </tr>
            </thead>
            <tbody>
              ${sensorData.map(
                (sensor) => `
                  <tr>
                    <td>${sensor.sensor}</td>
                    <td>${sensor.data.moisture}</td>
                    <td>${sensor.data.temperature}</td>
                  </tr>
                `
              )}
            </tbody>
          </table>
        </body>
      </html>
    `;
    res.status(200).send(html);
  } catch (err) {
    console.error(err);
    res.status(500).send('Error displaying sensor data');
  }
});

// Start the Express server
app.listen(3000, () => {
  console.log('Server listening on port 3000');
});
