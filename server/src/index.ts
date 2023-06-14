import express from 'express';
import bodyParser from 'body-parser';
import mongoose, { ConnectOptions } from 'mongoose';
import path from 'path';
import bot from './telegramBot';

bot;

const TELEGRAM_CHAT_ID = process.env.TELEGRAM_CHAT_ID as string;

// function to show the sensor date on the web page as YYYY-MM-DD HH:MM:SS
function formatDate(date: Date) {
  // increase 2 hours to the date
  let auxDate = new Date(date);
  // auxDate.setHours(auxDate.getHours() + 2);
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

interface EventData {
  eventType: string;
  comment: string;
  createdAt: Date;
}

interface Event {
  [key: string]: EventData;
}

interface EventDocument extends mongoose.Document {
  eventType: string;
  comment: string;
  createdAt: Date;
}

const eventSchema = new mongoose.Schema({
  eventType: {
    type: String,
    required: true,
    enum: ['water', 'battery', 'deepSleep'],
  },
  comment: {
    type: String,
    required: true,
  },
  createdAt: {
    type: Date,
    default: Date.now,
  },
})

const EventModel = mongoose.model<EventDocument>('Event', eventSchema);

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
    // Save the sensors data in the database
    const sensorDataArray = req.body;
    const date = new Date();
    for (const sensorData of sensorDataArray) {
      const { sensor, data } = sensorData;
      await SensorModel.create({ sensor, date, data });
    }

    // Send a response
    res.status(200).send('Sensors data received');
  } catch (err) {
    console.error(err);
    res.status(500).send('Error saving sensors data');
  }
});

// Define the route to receive event data
app.post('/send-event', async (req, res) => {
  try {
    const { eventType, comment } = req.body;
    if (!eventType || !comment) {
      return res.status(400).send('Missing required fields');
    }
    await EventModel.create({ eventType, comment });

    if (eventType === 'water') {
      const lastSensorData = await SensorModel.findOne().sort('-date');
      if (lastSensorData) {
        lastSensorData.data = {
          ...lastSensorData.data,
          watered: lastSensorData.data.moisture,
        };
        await lastSensorData.save();
      }
    }
    // Send a response
    res.status(200).send('Event saved successfully');
  } catch (err) {
    console.error(err);
    res.status(500).send('Error saving event');
  }
});

app.post('/alerts', async (req, res) => {
  try {
    const { message } = req.query as { message: string };
    if (!message) {
      return res.status(400).send('Missing required fields');
    } else {
      bot.sendMessage(TELEGRAM_CHAT_ID, message);
    }
  } catch (e) {
    console.error(e);
  }
});

// Try to make some magic to serve the React app
const reactSitePath = path.join(__dirname, '../site');
app.use(express.static(reactSitePath));
app.get('/react', async (req, res) => {
  res.sendFile(path.join(reactSitePath, 'index.html'));
});

// Define the route to display sensor data
app.get('/', async (req, res) => {
  try {
    // Get all the sensor data from the database
    const lastWeek = new Date();
    lastWeek.setDate(lastWeek.getDate() - 7); // get the date from a week ago
    const sensorData = await SensorModel.find({ date: { $gte: lastWeek } }).sort('-date');

    // Get all events data from the database
    const eventData = await EventModel.find().sort('-createdAt');

    // Create arrays to hold the sensor data
    const moistureData = [] as number[];
    const wateredData = [] as number[];
    const dateData = [] as string[];

    // Add the sensor data to the arrays
    sensorData.forEach((sensor) => {
      sensor.data.moisture as number > 550 ?
        moistureData.push(sensor.data.moisture as number) :
        moistureData.push(550);
      wateredData.push(sensor.data.watered as number);
      dateData.push(formatDate(sensor.date));
    });

    // Display the sensor data on a webpage
    const html = `
      <html>
        <head>
          <title>Plant Party</title>
          <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
          <script src="https://code.jquery.com/jquery-3.6.4.slim.min.js"></script>
          <link rel="stylesheet" href="https://cdn.datatables.net/1.13.4/css/jquery.dataTables.css" />
          <!-- Font Awesome CDN -->
          <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css" crossorigin="anonymous" referrerpolicy="no-referrer" />
          <script src="//cdn.datatables.net/1.13.4/js/jquery.dataTables.min.js"></script>
          <style>
            body {
              background-color: #effaed;
              background-image: url("https://www.transparenttextures.com/patterns/asfalt-dark.png");
              font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
              color: #444;
            }
            h1, h2 {
              color: #1B5E20;
              text-align: center;
              text-shadow: 1px 1px #9CCC65;
            }
            table {
              margin: auto;
              border-collapse: collapse;
              box-shadow: 0 0 20px rgba(0, 0, 0, 0.15);
              font-size: 1rem;
            }
            th, td {
              padding: 0.5rem;
              border: 1px solid #ddd;
            }
            th {
              background-color: #1B5E20;
              color: #fff;
              text-align: left;
            }
            tr:nth-child(even) {
              background-color: #f2f2f2;
            }
            tr:hover {
              background-color: #f5f5f5;
            }
          </style>
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
                mode: 'lines+markers',
                type: 'scatter',
                name: 'Watered',
                line: {
                  width: 1,
                  dash: 'dot',
                  shape: 'spline'
                },
                marker: {
                  size: 10,
                  symbol: 'star'
                }
              };

            var data = [moisture, watering];

            var layout = {
              shapes: [
                /*{
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
                }*/
              ],
              paper_bgcolor: 'rgba(0,0,0,0)'
            };

            Plotly.newPlot('graph', data, layout);
          </script>
          <h2>Sensors table</h2>
          <table id="sensorTable">
            <thead>
              <tr>
                <th>Sensor</th>
                <th>Moisture</th>
                <th>Date</th>
              </tr>
            </thead>
            <tbody>${sensorData.map((sensor) => ` <tr>
                <td>${sensor.sensor}</td>
                <td>${sensor.data.moisture ? sensor.data.moisture : 'WATERING'}</td>
                <td>${formatDate(sensor.date)}</td>
              </tr>`).join('')} </tbody>
          </table>
          <h2>Events table</h2>
          <table id="eventsTable">
            <thead>
              <tr>
                <th></th>
                <th>Comment</th>
                <th>Date</th>
              </tr>
            </thead>
            <tbody>${eventData.map((event) => ` <tr>
                <td>${event.eventType === 'water' ? '<i class="fas fa-tint"></i>' : event.eventType === 'battery' ? '<i class="fas fa-battery-full"></i>' : '<i class="fas fa-laptop"></i>'}</td>
                <td>${event.comment}</td>
                <td>${formatDate(event.createdAt)}</td>
              </tr>`).join('')} </tbody>
          </table>
          <script>
            $(document).ready(function() {
              $('#sensorTable').DataTable();
              $('#eventsTable').DataTable();
            });
          </script>
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
