const knex = require('./knex')

function createUser(user){
    return knex('userCredentials').insert(user);
}

function getUser(id){
    return knex('userCredentials').where('id', id).select()
}

function getAllUsers(){
    return knex('userCredentials').select('*')
}

function deleteUser(id){
    return knex('userCredentials').where('id', id).del()
}

function updateUser(id, user){
    return knex('userCredentials').where('id', id).update(user)
}

module.exports = {
    createUser,
    getUser,
    getAllUsers,
    deleteUser,
    updateUser
}