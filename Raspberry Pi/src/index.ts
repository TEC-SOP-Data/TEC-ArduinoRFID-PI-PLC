// This code imports the Express and node-snap7 libraries
import express from 'express'
import snap7 from 'node-snap7'

// -----------------------------
// DO NOT EDIT ABOVE THIS LINE

// This object maps the RFID tag of a vehicle to its corresponding number
const vehicleTag: { [key: string]: number } = {
    // To add a new vehicle, add a new line with the following format:
    // "TagUID": VehicleNumber
    "e067eb19":  9
}

// The port number for the web server
const WebServerPort = 3000

// The IP address of the PLC
const PLCIP = "192.168.0.1"

// The number of the Datablock in the PLC
const DBNumber = 1
// DO NOT EDIT BELOW THIS LINE
// ---------------------------

// Connect to the PLC using the node-snap7 library
const client = new snap7.S7Client()

// Create an instance of the Express application
const app = express()

// Define an endpoint for handling GET requests to the web server
app.get('/:station/:vehicleId', (req, res) => {
    // Connect to the PLC
    client.ConnectTo(PLCIP, 0, 1,(err)=>{
        // Check if there was an error or if the connection was successful
        if (!client.Connected() || err){
            // Log an error message if the connection was not successful
            console.error('Tag scanned while not connected to plc', err)
            return
        }
        // Log the parameters passed in the request
        console.log(req.params)

        // Convert the vehicle ID to a 2-byte buffer
        const buffer = Buffer.alloc(2)
        buffer.writeUint16BE(vehicleTag[req.params.vehicleId] || 0)
        console.log(buffer)
        
        // Parse the station number from the request parameters
        const station = parseInt(req.params.station)

        // Check if the station number is valid
        if (station <= 0) { return }
        
        // Write the vehicle ID to the PLC at the specified station
        client.DBWrite(DBNumber, (station - 1) * 2, 2, buffer)

        // Disconnect from the PLC
        client.Disconnect()
    })
})

// Start the web server and listen for incoming requests
app.listen(WebServerPort, () => {
    console.log('Webserver running on port:' + WebServerPort)
})
