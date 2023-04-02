# Base image
FROM node:14

# Set the working directory
WORKDIR /app

# Copy the package.json and package-lock.json files
COPY package*.json ./

# Install the dependencies
RUN npm install

# Copy the source code
COPY . .

# Build the TypeScript code
RUN npm run build

# Expose port 3000 for the Node.js server
EXPOSE 3000

# Set the environment variables
ENV MONGO_URI mongodb://mongodb:27017/sensors
ENV NODE_ENV production

# Start the Node.js server
CMD [ "npm", "start" ]

# MongoDB image
FROM mongo:4.4

# Set the environment variable for the database name
ENV MONGO_INITDB_DATABASE sensors

# Copy the initialization script
COPY init-mongo.js /docker-entrypoint-initdb.d/

# Expose port 27017 for MongoDB
EXPOSE 27017
