package com.example.oiler_bt_android

import BluetoothViewModel
import android.os.Build
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
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
            var permissionsGranted by remember { mutableStateOf(false) }

            RequestBluetoothPermissions { granted ->
                permissionsGranted = granted
            }

            if (permissionsGranted) {
                BluetoothLEDControlScreen()  // Only show screen after permissions are granted
            } else {
                Text("Bluetooth permissions required")
            }
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
