const knex = require('knex')

const usersdb = knex({
    client:'sqlite3',
    connection: {
        filename: 'db.sqlite3'
    }
})

const measureddb = knex({
    client:'sqlite3',
    connection: {
        filename: 'msrdb.sqlite3'
    }
})

module.exports = {usersdb, measureddb}