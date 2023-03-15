import express from 'express'
import snap7 from 'node-snap7'

const client = new snap7.S7Client()

const app = express()
const port = 3000

const vehicleTag: { [key: string]: number } = {
    "e067eb19":  9
}

app.get('/:station/:vehicleId', (req, res) => {
    client.ConnectTo('192.168.0.1', 0, 1,(err)=>{
        if (!client.Connected() || err){
            console.error('Tag scanned while not connected to plc', err)
            return
        }
        console.log(req.params)

        const buffer = Buffer.alloc(2)
        buffer.writeUint16BE(vehicleTag[req.params.vehicleId] || 0)
        console.log(buffer)
        
        const station = parseInt(req.params.station)

        if (station <= 0) { return }
        
        client.DBWrite(1, (station - 1) * 2, 2, buffer)
        client.Disconnect()
    })
})

app.listen(port, () => {
    console.log('Example app on port:' + port)
})