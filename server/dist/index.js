"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const express_1 = __importDefault(require("express"));
const body_parser_1 = __importDefault(require("body-parser"));
const mongoose_1 = __importDefault(require("mongoose"));
// Create the MongoDB model
const SensorModel = mongoose_1.default.model('Sensor', new mongoose_1.default.Schema({
    sensor: String,
    data: Object,
}));
// Connect to the MongoDB database
mongoose_1.default.connect('mongodb://localhost:27017/sensors', {
    useNewUrlParser: true,
    useUnifiedTopology: true,
});
// Create the Express app
const app = (0, express_1.default)();
// Use body-parser middleware to parse JSON request bodies
app.use(body_parser_1.default.json());
// Define the route to receive sensor data
app.post('/send-data', (req, res) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        // Save the sensor data to the database
        const { sensor, data } = req.body;
        yield SensorModel.create({ sensor, data });
        // Send a response
        res.status(200).send('Sensor data received');
    }
    catch (err) {
        console.error(err);
        res.status(500).send('Error saving sensor data');
    }
}));
// Define the route to display sensor data
app.get('/show-data', (req, res) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        // Get all the sensor data from the database
        const sensorData = yield SensorModel.find();
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
              ${sensorData.map((sensor) => `
                  <tr>
                    <td>${sensor.sensor}</td>
                    <td>${sensor.data.moisture}</td>
                    <td>${sensor.data.temperature}</td>
                  </tr>
                `)}
            </tbody>
          </table>
        </body>
      </html>
    `;
        res.status(200).send(html);
    }
    catch (err) {
        console.error(err);
        res.status(500).send('Error displaying sensor data');
    }
}));
// Start the Express server
app.listen(3000, () => {
    console.log('Server listening on port 3000');
});
