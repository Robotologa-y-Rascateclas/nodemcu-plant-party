import TelegramBot, { Message, KeyboardButton } from 'node-telegram-bot-api';
import axios from 'axios';
import { config } from 'dotenv';

config();

const TELEGRAM_BOT_TOKEN = process.env.TELEGRAM_BOT_TOKEN as string;
const SERVER_URL = process.env.SERVER_URL;
const TELEGRAM_CHAT_ID = process.env.TELEGRAM_CHAT_ID as string;

const bot = new TelegramBot(TELEGRAM_BOT_TOKEN, { polling: true });

console.log('Telegram bot started');

bot.on('message', (msg: Message) => {
  // const chatId = msg.chat.id;

  // Create the custom keyboard with the desired buttons
  const keyboard: KeyboardButton[][] = [
    [{ text: 'Watering' }, { text: 'Changing battery' }],
    [{ text: 'DeepSleep 10min' }, { text: 'DeepSleep 30min' }, { text: 'DeepSleep 60min' }],
  ];

  // Send the keyboard to the user
  bot.sendMessage(TELEGRAM_CHAT_ID, 'Aye, captain!', { reply_markup: { keyboard, resize_keyboard: true } });

  if (msg.text === 'Watering') {
    // Send an event to the server when the "Watering" button is pressed
    axios.post(`${SERVER_URL}/send-event`, {
      eventType: 'water',
      comment: 'Watering event triggered from Telegram Bot'
    });
  } else if (msg.text === 'Changing battery') {
    // Send an event to the server when the "Changing battery" button is pressed
    axios.post(`${SERVER_URL}/send-event`, {
      eventType: 'battery',
      comment: 'Battery change event triggered from Telegram Bot'
    });
  } else if (msg.text === 'DeepSleep 10min') {
    // Send an event to the server when the "DeepSleep 10min" button is pressed
    axios.post(`${SERVER_URL}/send-event`, {
      eventType: 'deepSleep',
      comment: 'Deep sleep event (10 min) triggered from Telegram Bot'
    });
  } else if (msg.text === 'DeepSleep 30min') {
    // Send an event to the server when the "DeepSleep 30min" button is pressed
    axios.post(`${SERVER_URL}/send-event`, {
      eventType: 'deepSleep',
      comment: 'Deep sleep event (30 min) triggered from Telegram Bot'
    });
  } else if (msg.text === 'DeepSleep 60min') {
    // Send an event to the server when the "DeepSleep 60min" button is pressed
    axios.post(`${SERVER_URL}/send-event`, {
      eventType: 'deepSleep',
      comment: 'Deep sleep event (60 min) triggered from Telegram Bot'
    });
  }
});

export default bot;
