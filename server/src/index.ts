import express from 'express';
import bodyParser from 'body-parser';
import mongoose, { ConnectOptions } from 'mongoose';

// function to show the sensor date on the web page as YYYY-MM-DD HH:MM:SS
function formatDate(date: Date) {
  // increase 2 hours to the date
  let auxDate = new Date(date);
  auxDate.setHours(auxDate.getHours() + 2);
  const year = auxDate.getFullYear();
  const month = auxDate.getMonth() + 1 < 10 ? `0${auxDate.getMonth() + 1}` : auxDate.getMonth() + 1;
  const day = auxDate.getDate() < 10 ? `0${auxDate.getDate()}` : auxDate.getDate();
  const hours = auxDate.getHours() < 10 ? `0${auxDate.getHours()}` : auxDate.getHours();
  const minutes = auxDate.getMinutes() < 10 ? `0${auxDate.getMinutes()}` : auxDate.getMinutes();
  const seconds = auxDate.getSeconds() < 10 ? `0${auxDate.getSeconds()}` : auxDate.getSeconds();
  return `${year}-${month}-${day} ${hours}:${minutes}:${seconds}`;
}

// Define the schema for the sensor data
interface SensorData {
  moisture?: number;
  watered?: number;
}

// Define the schema for the sensor
interface Sensor {
  [key: string]: SensorData;
}

// Define the schema for the MongoDB document
interface SensorDocument extends mongoose.Document {
  sensor: string;
  date: Date;
  data: Sensor;
}

// Create the MongoDB model
const SensorModel = mongoose.model<SensorDocument>(
  'Sensor',
  new mongoose.Schema({
    sensor: String,
    date: Date,
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
    const date = new Date();
    await SensorModel.create({ sensor, date, data });

    // Send a response
    res.status(200).send('Sensor data received');
  } catch (err) {
    console.error(err);
    res.status(500).send('Error saving sensor data');
  }
});

// Define the route to display sensor data
app.get('/', async (req, res) => {
  try {
    // Get all the sensor data from the database
    const sensorData = await SensorModel.find().sort('-date');

    // Create arrays to hold the sensor data
    const moistureData = [] as SensorData[];
    const wateredData = [] as SensorData[];
    const dateData = [] as string[];

    // Add the sensor data to the arrays
    sensorData.forEach((sensor) => {
      moistureData.push(sensor.data.moisture);
      wateredData.push(sensor.data.watered);
      dateData.push(formatDate(sensor.date));
    });

    // Display the sensor data on a webpage
    const html = `
      <html>
        <head>
          <title>Plant Party</title>
          <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
        </head>
        <body>
          <h1>¡Plant Party Sensor Data!</h1>
          <h2>Graph</h2>
          <div id="graph"></div>
          <script>
            var moistureData = ${JSON.stringify(moistureData)};
            var wateredData = ${JSON.stringify(wateredData)};
            var dateData = ${JSON.stringify(dateData)};

            var moisture = {
              x: dateData,
              y: moistureData,
              mode: 'lines+markers',
              type: 'scatter',
              name: 'Moisture',
              line: {
                width: 1,
                dash: 'dashdot'
              },
              marker: {
                size: 4
              }
            };

            var watering = {
                x: dateData,
                y: wateredData,
                mode: 'markers',
                type: 'scatter',
                name: 'Watered',
                marker: {
                  size: 10,
                  symbol: 'star'
                }
              };

            var data = [moisture, watering];

            var layout = {
              shapes: [
                {
                  type: 'line',
                  xref: 'paper',
                  x0: 0,
                  y0: 680.0,
                  x1: 1,
                  y1: 680.0,
                  line:{
                    color: 'rgb(255, 150, 150)',
                    width: 1,
                    dash:'dot'
                  }
                }
              ]
            };

            Plotly.newPlot('graph', data, layout);
          </script>
          <h2>Table</h2>
          <table>
            <thead>
              <tr>
                <th>Sensor</th>
                <th>Moisture</th>
                <th>Date</th>
              </tr>
            </thead>
            <tbody>
              ${sensorData.map(
                (sensor) => `
                  <tr>
                    <td>${sensor.sensor}</td>
                    <td>${sensor.data.moisture ? sensor.data.moisture : 'WATERING'}</td>
                    <td>${formatDate(sensor.date)}</td>
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

// Define the route to display sensor data as JSON
app.get('/json', async (req, res) => {
  try {
    const { sensor, from, to, sort, limit } = req.query as {
      sensor?: string;
      from?: string;
      to?: string;
      sort?: string;
      limit?: string;
    };
    const filters = {} as any;

    // Add the sensor filter if it exists
    if (sensor) {
      filters.sensor = sensor;
    }

    // If 'from' is provided, add it to the filters
    if (from) {
      filters.timestamp = { $gte: new Date(from) };
    }

    // If 'to' is provided, add it to the filters
    if (to) {
      filters.timestamp = { ...filters.timestamp, $lte: new Date(to) };
    }

    let sortParam = '-date';
    // If 'sort' is provided, add it to the filters
    if (sort) {
      sortParam = sort;
    }

    let limitParam = 0;
    // If 'limit' is provided, add it to the filters
    if (limit) {
      limitParam = parseInt(limit);
    }

    const projection = { _id: 0, __v: 0 };

    // Get all the sensor data from the database
    const sensorData = await SensorModel
      .find(filters, projection)
      .sort(sortParam)
      .limit(limitParam);

    // Send the sensor data as JSON
    res.status(200).json(sensorData);
  } catch (err) {
    console.error(err);
    res.status(500).send('Error displaying sensor data');
  }
});

// Start the Express server
app.listen(3000, () => {
  console.log('Server listening on port 3000');
});
