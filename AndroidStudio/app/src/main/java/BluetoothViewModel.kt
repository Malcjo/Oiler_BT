import android.app.Application
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import androidx.lifecycle.AndroidViewModel
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.io.IOException
import java.io.OutputStream
import java.util.*

class BluetoothViewModel(application: Application) : AndroidViewModel(application) {

    private val bluetoothAdapter: BluetoothAdapter? = BluetoothAdapter.getDefaultAdapter()
    private var bluetoothSocket: BluetoothSocket? = null
    private var outputStream: OutputStream? = null
    private val uuid: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB") // Standard SPP UUID

    // List of paired devices
    val pairedDevices: Set<BluetoothDevice>?
        get() = bluetoothAdapter?.bondedDevices

    fun connectToDevice(device: BluetoothDevice) {
        CoroutineScope(Dispatchers.IO).launch {
            try {
                bluetoothSocket = device.createRfcommSocketToServiceRecord(uuid)
                bluetoothSocket?.connect()
                outputStream = bluetoothSocket?.outputStream
                println("Connected to ${device.name}")
            } catch (e: IOException) {
                println("Error: ${e.message}")
            }
        }
    }

    fun turnOnLED() {
        sendData("H")
    }

    fun turnOffLED() {
        sendData("L")
    }

    private fun sendData(data: String) {
        CoroutineScope(Dispatchers.IO).launch {
            try {
                outputStream?.write(data.toByteArray())
                println("Sent: $data")
            } catch (e: IOException) {
                println("Error: ${e.message}")
            }
        }
    }

    override fun onCleared() {
        super.onCleared()
        try {
            bluetoothSocket?.close()
        } catch (e: IOException) {
            println("Error closing socket: ${e.message}")
        }
    }
}
