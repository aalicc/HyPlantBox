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

//loop for sending humidity values
interval1 = setInterval(() => {
    if (dataFlag) {
        return
    }
    if (!dataFlag){
        sendHumidity()
    }
}, 10000)


//serial connection
const port = new SerialPort({
    path:'/dev/ttyS0',
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
    let name = req.user.name
    res.render('index.ejs', {name})
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
        dataFlag = true
        const hashedPassword = await bcrypt.hash(req.body.password, 10)
        await knex.createThingy(knex.usersdb, 'userCredentials',{
            id: Date.now().toString(),
            email: req.body.email,
            name: req.body.name,
            password: hashedPassword
        })
        dataFlag = false
        res.redirect('/login')
    } catch {
        res.redirect('/register')
    }
})

app.get('/values', async (req, res) => { //change the order of items //WARNING: if you send empty values from control page the website crashes
    let ruuvi1Q = await knex.ruuvidata('ruuvi1').max('time').select('Temp', 'Hum')
    let ruuviProQ = await knex.ruuvidata('ruuviPro').max('time').select('Temp', 'Hum')
    let ruuviValues = ruuvi1Q[0].Temp + '&' + ruuviProQ[0].Temp + 
    '&' + ruuvi1Q[0].Hum + '&' + ruuviProQ[0].Hum + '&'
    let values = ruuviValues.toString();
    if (valuesCon !== undefined) {
    values += valuesCon.toString();
    } 
    const response = {
        value: values
    }

    res.json(response)
})

app.get('/control', checkAuthenticated, (req,res) => {
    dataFlag = true
    res.render('control.ejs')
})

app.post('/control', async (req,res) => {
    try{
        let stringy = 's' + ',' + req.body.fanspeed + ',' + req.body.lph + ',' + req.body.hph + ',' + req.body.lTDS + ',' + req.body.hTDS + 
         ',' + req.body.ontime + ',' + req.body.offtime + ',' + req.body.hhum + ','  + req.body.htemp
        let arr = stringy.split(',')
        clearInterval(interval1)
        port.write(stringy, (err) => {
            if (err) {
                console.log('Didnt work')
            }
            else {
                console.log('Succccess')
            }
        })
        dataFlag = false

        interval1 = setInterval(() => {
            if (dataFlag) {
                return
            }
            if (!dataFlag){
                sendHumidity()
            }
        }, 10000)

        res.redirect('/home')
    }
    catch{
        res.redirect('/home')
    } 
})

app.get('/logout', checkAuthenticated, (req, res) => {
    req.logOut(function(err) {
        if (err) { return next(err); }
        res.redirect('/')
    })
})

//functions
const sendHumidity = async () => {
    let ruuvi1Q = await knex.ruuvidata('ruuvi1').max('time').select('Temp', 'Hum')
    let ruuviProQ = await knex.ruuvidata('ruuviPro').max('time').select('Temp', 'Hum')
    let stringy = 'r, ' + ruuvi1Q[0].Temp + ', ' + ruuvi1Q[0].Hum + ', ' + ruuviProQ[0].Temp + ', ' + ruuviProQ[0].Hum
    let arr1 = stringy.split(',')
    port.write(stringy, (err) => {
        if (err) {
            console.log('Humidity Didnt work')
        }
        else {
            console.log('Succccess on Humidity')
            console.log(stringy)
        }
    })
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