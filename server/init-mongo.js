db = db.getSiblingDB('sensors');

db.createUser({
  user: 'sensorUser',
  pwd: 'sensorPassword',
  roles: [{ role: 'readWrite', db: 'sensors' }],
});
