if (process.env.NODE_ENV !== 'production') {
    require('dotenv').config()
}

//CHANGE RASPBERRY PI SERIAL CONFIG SETTINGS BEFORE RUNNING

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


//variables
let users = []
let valuesCon
let dataFlag = false
let interval1

const initializePassport = require('./passport-config')
const path = require("path");
const { Readline } = require('readline/promises')
const { data } = require('jquery')
initializePassport(
    passport,
    email => users.find(user => user.email === email),
    id => users.find(user => user.id === id)
)


interval1 = setInterval(() => {
    if (dataFlag) {
        return
    }
    if (!dataFlag){
        sendHumidity()
    }
}, 5000)


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
      port.pipe(parser)
      // Read data from the serial port
      parser.on('data', (line) => {
        valuesCon = line
      })
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
app.get('/', checkNotAuthenticated,(req, res) =>{
    res.redirect('/login')
})

app.get('/home', checkAuthenticated, (req, res) =>{
    res.render('index.ejs')
})

app.get('/login', checkNotAuthenticated, async (req,res) =>{
    users = await knex.getAll(knex.usersdb, 'userCredentials')
    res.render('login.ejs')
})

app.post('/login', checkNotAuthenticated, passport.authenticate('local', {
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
        await knex.createThingy(knex.usersdb, 'userCredentials',{
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

app.get('/control', (req,res) => {
    res.render('control.ejs')
})

app.post('/control', async (req,res) => {
    try{
        dataFlag = true
        let ruuvi1Q = await knex.ruuvidata('ruuvi1').max('time').select('Temp', 'Hum')
        let stringy = 's, 70, 100, 3, 4, 5, 6, 7, 8' //settings have to be here instead of ruuvi data
        let arr = stringy.split(',') //to send humidity data constantly a loop is needed which will be paused for the duration of this here post route function
        let i = 0
        console.log(arr)
        clearInterval(interval1)
        setInterval(() => {
            if (i <= 8){ //replace with amount of parameteres -1 so that the app does not crash
                port.write(arr[i], (err) => {
                    if (err) {
                        console.log('Didnt work')
                    }
                    else {
                        console.log('Succccess')
                        console.log(arr[i])
                        i++
                    }
                })
            }

        }, 58)
        i = 0
        res.redirect('/control')
        dataFlag = false

        interval1 = setInterval(() => {
            if (dataFlag) {
                return
            }
            if (!dataFlag){
                sendHumidity()
            }
        }, 5000)
    }
    catch{
        res.redirect('/control')
    } 
})

app.get('/logout', checkAuthenticated, (req, res) => {
    req.logOut(function(err) {
        if (err) { return next(err); }
        res.redirect('/')
    })
})

const sendHumidity = async () => {
    let ruuvi1Q = await knex.ruuvidata('ruuvi1').max('time').select('Temp', 'Hum')
    let ruuviProQ = await knex.ruuvidata('ruuviPro').max('time').select('Temp', 'Hum')
    let stringy = 'r, ' + ruuvi1Q[0].Temp + ', ' + ruuvi1Q[0].Hum + ', ' + ruuviProQ[0].Temp + ', ' + ruuviProQ[0].Hum
    let arr1 = stringy.split(',')
    let i = 0
    //console.log(arr1)
    setInterval(() => {
        if (i <= 4){ //replace with amount of parameteres -1 so that the app does not crash
            port.write(arr1[i], (err) => {
                if (err) {
                    console.log('Didnt work')
                }
                else {
                    console.log('Succccess')
                    console.log(arr1[i])
                    i++
                }
            })
        }
    }, 58)
    i = 0
}

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