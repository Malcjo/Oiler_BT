import android.annotation.SuppressLint
import android.app.Application
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.util.Log
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
    private val bluetoothDeviceName = "Your Bluetooth Device Name" // Change this to your device name
    private val uuid: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB") // Standard SPP UUID

    init {
        connectToBluetoothDevice()
    }

    @SuppressLint("MissingPermission")
    private fun connectToBluetoothDevice() {
        bluetoothAdapter?.bondedDevices?.forEach { device ->
            if (device.name == bluetoothDeviceName) {
                connect(device)
                return@forEach
            }
        }
    }

    @SuppressLint("MissingPermission")
    private fun connect(device: BluetoothDevice) {
        CoroutineScope(Dispatchers.IO).launch {
            try {
                bluetoothSocket = device.createRfcommSocketToServiceRecord(uuid)
                bluetoothSocket?.connect()
                outputStream = bluetoothSocket?.outputStream
                Log.d("Bluetooth", "Connected to ${device.name}")
            } catch (e: IOException) {
                Log.e("Bluetooth", "Connection failed", e)
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
                Log.d("Bluetooth", "Sent: $data")
            } catch (e: IOException) {
                Log.e("Bluetooth", "Error sending data", e)
            }
        }
    }

    override fun onCleared() {
        super.onCleared()
        try {
            bluetoothSocket?.close()
        } catch (e: IOException) {
            Log.e("Bluetooth", "Error closing socket", e)
        }
    }
}
