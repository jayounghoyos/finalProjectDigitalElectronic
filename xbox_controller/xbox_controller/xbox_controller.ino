#include "BluetoothSerial.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"

// Variable para manejar el escaneo
bool scanning = false;

// Callback para manejar dispositivos detectados
void btCallback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
  if (event == ESP_BT_GAP_DISC_RES_EVT) { // Evento de dispositivo encontrado
    char bda_str[18];
    sprintf(bda_str, "%02x:%02x:%02x:%02x:%02x:%02x",
            param->disc_res.bda[0], param->disc_res.bda[1],
            param->disc_res.bda[2], param->disc_res.bda[3],
            param->disc_res.bda[4], param->disc_res.bda[5]);

    Serial.print("Dispositivo encontrado: Dirección: ");
    Serial.println(bda_str);

    // Buscar el nombre del dispositivo
    for (int i = 0; i < param->disc_res.num_prop; i++) {
      if (param->disc_res.prop[i].type == ESP_BT_GAP_DEV_PROP_BDNAME) {
        Serial.print("Nombre: ");
        Serial.println((char *)param->disc_res.prop[i].val);
      }
    }
  } else if (event == ESP_BT_GAP_DISC_STATE_CHANGED_EVT) { // Escaneo terminado
    if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED) {
      Serial.println("Escaneo terminado.");
      scanning = false;
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Bluetooth...");

  // Inicializar Bluetooth
  if (!btStart()) {
    Serial.println("Error iniciando Bluetooth");
    return;
  }
  esp_bluedroid_init();
  esp_bluedroid_enable();

  // Registrar el callback para manejar eventos
  esp_bt_gap_register_callback(btCallback);

  // Iniciar escaneo
  Serial.println("Iniciando escaneo de dispositivos Bluetooth...");
  esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0); // Escanear por 10 segundos
  scanning = true;
}

void loop() {
  // Reiniciar el escaneo si no está activo
  if (!scanning) {
    Serial.println("Reiniciando escaneo...");
    esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
    scanning = true;
  }
  delay(1000);
}
