package com.estacionamento.api.controller;

import com.estacionamento.api.dao.SensorDAO;
import com.estacionamento.api.models.Sensor;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/smartvagas")
public class SensorController {

    private final SensorDAO sensorDAO = new SensorDAO();

    @PostMapping("/sensores")
    public String criarSensor(@RequestBody Sensor sensor) {
        sensorDAO.cadastrarSensor(sensor);

        return "Sensor " + sensor.getNome() + " cadastrado com sucesso!";
    }

    @GetMapping("/sensores")
    public List<Sensor> listarSensores() {
        return sensorDAO.listarSensores();
    }

    @GetMapping("/sensores/{id}")
    public Sensor buscarSensorPorId(@PathVariable int id) {
        return sensorDAO.buscarSensorPorId(id);
    }

    @PutMapping("/sensores/{id}")
    public String atualizarSensor(@PathVariable int id, @RequestBody Sensor sensorAtualizado) {
        Sensor existente = sensorDAO.buscarSensorPorId(id);

        if (existente != null) {
            sensorAtualizado.setId(id);
            sensorDAO.atualizarSensor(sensorAtualizado);

            return "Sensor atualizado com sucesso!";
        }
        return "Erro: Sensor não encontrado.";
    }

    @DeleteMapping("/sensores/{id}")
    public String deletarSensor(@PathVariable int id) {
        Sensor existente = sensorDAO.buscarSensorPorId(id);

        if (existente != null) {
            sensorDAO.deletarSensor(id);
            return "Sensor deletado com sucesso!";
        }
        return "Erro: Sensor não encontrado para deletar.";
    }
}
