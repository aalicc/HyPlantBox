if (process.env.NODE_ENV !== 'production') {
    require('dotenv').config()
}

//FYI CHANGE RASPBERRY PI SERIAL CONFIG SETTINGS

//packages
const express = require('express')
const app = express()
const bcrypt = require('bcrypt')
const passport = require('passport')
const flash = require('express-flash')
const session = require('express-session')
const methodOverride = require('method-override')
const bodyParser = require('body-parser')
const {SerialPort} = require('serialport')
const {ReadlineParser} = require('@serialport/parser-readline')
const knex = require('./db/knex.js')

const initializePassport = require('./passport-config')
const path = require("path");
const { Readline } = require('readline/promises')
initializePassport(
    passport,
    email => users.find(user => user.email === email),
    id => users.find(user => user.id === id)
)

//variables
let users = []
let valuesCon

//serial connection
const port = new SerialPort({
    path:'/dev/ttyS0', //try connecting via pins
    baudRate: 19200,
    autoOpen: false
})

const parser = new ReadlineParser()

port.open((err) => {
    if (err) {
      console.error('Error opening port:', err.message)
    } else {
      console.log('Serial port opened.')
      port.pipe(parser)             //ALL VARIABLES NOT NEEDED, SPLITTING HAPPENS IN FRONT END SCRIPT
      // Read data from the serial port
      parser.on('data', (line) => { //async maybe?
        valuesCon = line
      })

      // Write data to the serial port //THE FORMAT IS CORRECT DO NOT CHANGE
      const sendData = 'Hello, Controllino!'
      port.write(sendData, (err) => {
        if (err) {
          console.error('Error writing to port:', err)
        } else {
          console.log('Sent:', sendData)
        }
      })
    }
  })
  
//package settings
app.use(bodyParser.urlencoded({extended: false}))
app.use(bodyParser.json())
app.use(express.urlencoded({extended:false}))
app.use(flash())
app.use(session({
    secret: process.env.SESSION_SECRET,
    resave: false,
    saveUninitialized: false
}))
app.use(passport.initialize())
app.use(passport.session())
app.use(methodOverride('_method'))
app.use(express.static(__dirname + '/views'))
app.set('view engine', 'ejs')

//rest
app.get('/', (req, res) =>{
    res.redirect('/login')
})

app.get('/home', checkAuthenticated, (req, res) =>{
    res.render('index.ejs')
})

app.get('/login', async (req,res) =>{
    users = await knex.getAll(knex.usersdb, 'userCredentials')
    res.render('login.ejs')
})

app.post('/login', passport.authenticate('local', {
    successRedirect: '/home',
    failureRedirect: '/login',
    failureFlash: true
}))

app.get('/register', (req,res) =>{
    res.render('register.ejs')
})

app.post('/register', async (req,res) => {
    try {
        const hashedPassword = await bcrypt.hash(req.body.password, 10)
        await db.createUser({
            id: Date.now().toString(),
            name: req.body.name,
            email: req.body.email,
            password: hashedPassword
        })
        res.redirect('/login')
    } catch {
        res.redirect('/register')
    }
})

app.get('/values', async (req, res) => {
    let ruuvi1Q = await knex.ruuvidata('ruuvi1').max('time').select('Temp', 'Hum')
    let ruuviProQ = await knex.ruuvidata('ruuviPro').max('time').select('Temp', 'Hum')
    let ruuviValues = ruuvi1Q[0].Temp + '&' + ruuviProQ[0].Temp + 
    '&' + ruuvi1Q[0].Hum + '&' + ruuviProQ[0].Hum + '&'
    let values = ruuviValues.toString();
    if (valuesCon !== undefined) {
    values += valuesCon.toString();
    } 
    console.log(values)
    const response = {
        value: values
    }

    res.json(response)
})

app.delete('/logout', (req, res) => {
    req.logOut()
    res.redirect('/')
})

function checkAuthenticated(req, res, next) {
    if (req.isAuthenticated()) {
        return next()
    }

    res.redirect('/login')
}

function checkNotAuthenticated(req, res, next) {
    if (req.isAuthenticated()) {
        return res.redirect('/')
    }
    next()
}

app.listen(3000)