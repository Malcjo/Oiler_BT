package com.example.oiler_bt_android

import BluetoothViewModel
import android.bluetooth.BluetoothDevice
import android.os.Build
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.Button
import androidx.compose.material.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import androidx.activity.compose.rememberLauncherForActivityResult

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            BluetoothApp()
            /*
            var permissionsGranted by remember { mutableStateOf(false) }

            RequestBluetoothPermissions { granted ->
                permissionsGranted = granted
            }

            if (permissionsGranted) {

                BluetoothLEDControlScreen()  // Only show screen after permissions are granted
            } else {
                Text("Bluetooth permissions required")
            }
            */
        }
    }
}

@Composable
fun BluetoothApp(viewModel: BluetoothViewModel = viewModel()) {
    var permissionsGranted by remember { mutableStateOf(false) }
    var selectedDevice by remember { mutableStateOf<BluetoothDevice?>(null) }

    // Request Bluetooth permissions
    RequestBluetoothPermissions { granted ->
        permissionsGranted = granted
    }

    if (permissionsGranted) {
        if (selectedDevice == null) {
            // Show available devices if no device is selected
            AvailableDevicesScreen(viewModel) { device ->
                selectedDevice = device
                viewModel.connectToDevice(device)
            }
        } else {
            // Once a device is selected, show the LED control screen
            BluetoothLEDControlScreen(viewModel)
        }
    } else {
        Text("Bluetooth permissions required")
    }
}

@Composable
fun AvailableDevicesScreen(viewModel: BluetoothViewModel, onDeviceSelected: (BluetoothDevice) -> Unit) {
    val devices = viewModel.pairedDevices

    Column(modifier = Modifier.fillMaxSize().padding(16.dp)) {
        Text("Select a Bluetooth Device:")
        if (devices != null && devices.isNotEmpty()) {
            LazyColumn {
                items(devices.toList()) { device ->
                    Row(modifier = Modifier
                        .fillMaxWidth()
                        .padding(8.dp)
                        .clickable { onDeviceSelected(device) }) {
                        Text(device.name ?: "Unknown Device")
                    }
                }
            }
        } else {
            Text("No paired Bluetooth devices found.")
        }
    }
}




@Composable
fun BluetoothLEDControlScreen(viewModel: BluetoothViewModel = viewModel()) {
    Column(modifier = Modifier.fillMaxSize().padding(16.dp)) {
        Button(
            onClick = { viewModel.turnOnLED() },
            modifier = Modifier.padding(bottom = 8.dp)
        ) {
            Text(text = "Turn LED On")
        }

        Button(
            onClick = { viewModel.turnOffLED() }
        ) {
            Text(text = "Turn LED Off")
        }
    }
}

@Composable
fun RequestBluetoothPermissions(onPermissionResult: (Boolean) -> Unit) {
    // Only needed for Android 12 and above
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
        val permissions = arrayOf(
            android.Manifest.permission.BLUETOOTH_SCAN,
            android.Manifest.permission.BLUETOOTH_CONNECT
        )

        val permissionLauncher = rememberLauncherForActivityResult(
            ActivityResultContracts.RequestMultiplePermissions()
        ) { permissionsGranted ->
            val allGranted = permissionsGranted.values.all { it }
            onPermissionResult(allGranted)
        }

        LaunchedEffect(Unit) {
            permissionLauncher.launch(permissions)
        }
    } else {
        // If below Android 12, permissions are granted at install time
        onPermissionResult(true)
    }
}
