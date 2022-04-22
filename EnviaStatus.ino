void EnviaStatus() {
  Serial2.print(EstadoAtual[0]);
  Serial2.print("-");
  Serial2.print(EstadoAtual[1]);
  Serial2.print("-");
  Serial2.print(EstadoAtual[2]);
  Serial2.print("-");
  Serial2.print(perc);
  Serial2.print("-");
  int anguloint = angulo.toInt();
  Serial2.print(anguloint);
  Serial2.print("-");
  Serial2.print(tempo);
  Serial2.print("#");

  StaticJsonDocument<500> saida;
  saida["type"] = "status";
  saida["farm_id"] = NODE_ID;
  saida["node_name"] = PIVOT_ID;
  saida["payload"] = String( String(EstadoAtual[0]) + "-" + String(EstadoAtual[1]) + "-" + String(EstadoAtual[2]) + "-" + String(perc) + "-" + String(anguloint) + "-" + String(tempo));
//  saida["on_off"] = String(EstadoAtual[2]);
//  saida["front_back"] = String(EstadoAtual[0]);
//  saida["water"] = String(EstadoAtual[1]);
//  saida["percent"] = perc;
//  saida["angle"] = anguloint;
//  saida["timestamp"] = tempo;
//  saida["fail"] = 0;
  char out[500];
  int a = serializeJson(saida, Serial1);
//  int b = serializeJson(saida, out);
//  //    Serial2.print("bytes = ");
//  //    Serial2.println(b, DEC);
//  mqtt.publish("cloud", out);


}
