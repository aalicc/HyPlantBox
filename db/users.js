const knex = require('knex');
const { usersdb } = require('./knex');

function createUser(user){
    return usersdb('userCredentials').insert(user);
}

function getUser(id){
    return usersdb('userCredentials').where('id', id).select()
}

function getAllUsers(){
    return usersdb('userCredentials').select('*')
}

function deleteUser(id){
    return usersdb('userCredentials').where('id', id).del()
}

function updateUser(id, user){
    return usersdb('userCredentials').where('id', id).update(user)
}

module.exports = {
    createUser,
    getUser,
    getAllUsers,
    deleteUser,
    updateUser
}