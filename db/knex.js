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

const ruuvidata = knex({
    client:'sqlite3',
    connection: {
        filename: 'ruuvidata.sqlite'
    }
})

function createThingy(db, table, thingy){
    return db(table).insert(thingy)
}

function getThingy(db, table, id){
    return db(table).where('id', id).select()
}

function getAll(db, table){
    return db(table).select('*')
}

function deleteThingy(db, table, id){
    return db(table).where('id', id).del()
}

function updateThingy(db, table, id, thingy){
    return db(table).where('id', id).update(thingy)
}

module.exports = {
    measureddb,
    usersdb,
    ruuvidata,
    createThingy,
    getThingy,
    getAll,
    deleteThingy,
    updateThingy
}