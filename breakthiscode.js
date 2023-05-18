app.get('/api', async (req, res) => {
    devID = req.query.dev_id
    temp = req.query.inside_temp
    hum = req.query.hum


    const userr = await getMCU(devID) //get mcu id

    const dbTempOBJ = await knex.raw("SELECT inside_temp from Temps where dev_id = " + req.query.dev_id + " ORDER BY end_time DESC LIMIT 1")
    const dbTemp = dbTempOBJ[0].inside_temp

    if(temp >= dbTemp - 1 && temp <= dbTemp + 1){
        await knex.raw("UPDATE Temps set end_time = DATETIME('NOW') where end_time = " +
            "(SELECT end_time from Temps ORDER BY end_time DESC LIMIT 1)"
            + "" +
            ";")
    } else {
        await knex.raw("INSERT INTO Temps values (" + devID + ", " + temp + ", " + hum + ", DATETIME('NOW'), DATETIME('NOW'));")
    }

    let goingHomeObj = await knex.raw("SELECT goingHome, arrivalTime, reqTemp from userCredentials where id = (SELECT user_id from Devs where id = " + req.query.dev_id + ");")
    let going_home = goingHomeObj[0].goingHome
    const arrivalTime = goingHomeObj[0].arrivalTime
    const reqTemp = goingHomeObj[0].reqTemp

    console.log("here")
    console.log(going_home)
    console.log(arrivalTime)
    if(going_home){
        console.log("UPDATE")
        //await knex.raw("UPDATE Devs set heat_time = DATETIME('NOW') where id = " + req.query.dev_id + ";")
    }else if (arrivalTime){
        console.log("update 2")
        await knex.raw("UPDATE Devs set heat_time = datetime(\n" +
            "            julianday(\"" + arrivalTime + "\")\n" +
            "            -\n" +
            "            (\n" +
            "                    julianday((SELECT start_time FROM Temps where dev_id = " + devID + " AND inside_temp = " + reqTemp + " limit 1))\n" +
            "                    -\n" +
            "                    julianday((SELECT end_time FROM Temps where dev_id = " + devID + " AND inside_temp = " + temp + " AND end_time < (\n" +
            "                        SELECT start_time FROM Temps where dev_id = " + devID + " AND inside_temp = " + reqTemp + " limit 1\n" +
            "                        ) limit 1))\n" +
            "                )\n" +
            "    ) where id = " + devID + ";")
        await knex.raw("UPDATE userCredentials set arrivalTime = \"\" where id = (SELECT user_id from Devs where id = " + req.query.dev_id + ");")
    }

    const heatTimeObj = await knex.raw("SELECT heat_time from Devs where id = " + req.query.dev_id + ";")
    const heatTime = heatTimeObj[0].goingHome
    if(heatTime)
        await knex.raw("UPDATE userCredentials set goingHome = (case when (select heat_time from Devs where id = 1) < DATETIME(\"now\") then 1 else 0 end) where id = (SELECT user_id from Devs where id = " + req.query.dev_id + ");")

    goingHomeObj = await knex.raw("SELECT goingHome from userCredentials where id = (SELECT user_id from Devs where id = " + req.query.dev_id + ");")
    going_home = goingHomeObj[0].goingHome
    if(going_home){
        await knex.raw("UPDATE Devs set heat_time = \"\" where id =  " + req.query.dev_id + ";")
    }


    const id = await getUser(userr[0].user_id) //get user id

    const minTemp = id[0].minTemp
    const goingHome = id[0].goingHome
    const dataString = minTemp + '/' + reqTemp + '/' + goingHome + '/'
    res.send(dataString)
})