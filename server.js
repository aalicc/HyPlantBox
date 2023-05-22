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
const db = require('./db/users')
const {SerialPort} = require('serialport')
const {ReadlineParser} = require('@serialport/parser-readline')
const knex = require('./db/knex.js')
const raspi = require('raspi')
const Serial = require('raspi-serial').Serial


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

/*let level
let power
let pumps
let heater
let buzzer
let light1
let light2*/

let temp1//
let temp2//
let ph//
let hum//
let ec//
let tds//
let orp//
let co2//

//serial connection
const port = new SerialPort({ //if using gpio then switching to onoff library
    path:'/dev/ttyS0', //try connecting via pins
    baudRate: 9600,
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
      parser.on('data', (line) => { //async maybe?
        console.log(line)
        const arr = line.split('&')
        //all variables are for debugging
        temp1 = arr[0]  //needs SQL storage...and graph output, 
        temp2 = arr[1]
        ph = arr[2]
        hum = arr[3]
        ec = arr[4]
        tds = arr[5]
        orp = arr[6]
        co2 = arr[7]
        /*console.log(temp1)
        console.log(temp2)
        console.log(ph)
        console.log(hum)
        console.log(ec)
        console.log(tds)
        console.log(orp)
        console.log(co2)*/ // next goes graphs and C parsing
        this.dataArray = JSON.stringify(arr)
    
        knex.measureddb('Measurements').insert(
            [
                { temp1: arr[0] ,
                 temp2: arr[1] ,
                 ph: arr[2] ,
                 hum: arr[3] ,
                 ec: arr[4] , 
                 tds: arr[5] ,
                 orp: arr[6] ,
                 co2: arr[7] }
            ])
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
  
  /*        //code that might be needed later for reference/rework
port.on('data', (line) => { //async maybe?
    console.log )
    const arr = line.split('&')
    //all variables are for debugging
    temp1 = arr[0]  //needs SQL storage...and graph output, 
    temp2 = arr[1]
    ph = arr[2]
    hum = arr[3]
    ec = arr[4]
    tds = arr[5]
    orp = arr[6]
    co2 = arr[7]
    console.log(temp1)
    console.log(temp2)
    console.log(ph)
    console.log(hum)
    console.log(ec)
    console.log(tds)
    console.log(orp)
    console.log(co2) // next goes graphs and C parsing
    this.dataArray = JSON.stringify(arr)

    knex.measureddb('Measurements').insert(
        [
            { temp1: arr[0] ,
             temp2: arr[1] ,
             ph: arr[2] ,
             hum: arr[3] ,
             ec: arr[4] , 
             tds: arr[5] ,
             orp: arr[6] ,
             co2: arr[7] }
        ])
})*/

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
    users = await db.getAllUsers()
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

app.get('/cp', checkAuthenticated, (req,res) => {
    res.render('cp.ejs')
})

app.get('/hist', checkAuthenticated, (req,res) => {
    res.render('hist.ejs')
})

app.get('/abt', checkAuthenticated, (req,res) => {
    res.render('about.ejs')
})

app.delete('/logout', (req, res) => {
    req.logOut()
    res.redirect('/')
})

/*app.use(, (req,res) => {
    res.status(404).render('404.ejs')
})*/

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