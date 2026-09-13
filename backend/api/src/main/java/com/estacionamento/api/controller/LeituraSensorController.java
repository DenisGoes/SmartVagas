package com.estacionamento.api.controller;

import com.estacionamento.api.dao.LeituraSensorDAO;
import com.estacionamento.api.models.LeituraSensor;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/smartvagas")
public class LeituraSensorController {

    private final LeituraSensorDAO leituraSensorDAO = new LeituraSensorDAO();

    @PostMapping("/leituras")
    public ResponseEntity<String> receberLeituraEsp(@RequestBody LeituraSensor leituraSensor) {
        System.out.println("DADOS RECEBIDOS:");
        System.out.println("Sensor ID: " + leituraSensor.getSensorId());
        System.out.println("Estado: " + leituraSensor.getEstado());

        boolean sucesso = leituraSensorDAO.cadastrarLeituraSensor(leituraSensor);

        if (!sucesso) {
            return ResponseEntity.status(HttpStatus.NOT_FOUND).body("Sensor não encontrado.");
        }

        return ResponseEntity.ok("Leitura recebida com sucesso!");
    }

    @GetMapping("/leituras")
    public List<LeituraSensor> listarLeituras() {
        return leituraSensorDAO.listarLeituras();
    }

    @GetMapping("/leituras/{id}")
    public LeituraSensor buscarLeituraPorId(@PathVariable int id) {
        return leituraSensorDAO.buscarLeituraPorId(id);
    }

    @GetMapping("/leituras/sensor/{sensorId}")
    public List<LeituraSensor> listarLeiturasPorSensor(@PathVariable int sensorId) {
        return leituraSensorDAO.listarLeiturasPorSensor(sensorId);
    }

    @DeleteMapping("/leituras/{id}")
    public ResponseEntity<String> deletarLeitura(@PathVariable int id) {

        LeituraSensor existente = leituraSensorDAO.buscarLeituraPorId(id);

        if (existente == null) {
            return ResponseEntity
                    .status(HttpStatus.NOT_FOUND)
                    .body("Leitura não encontrada para deletar.");
        }

        boolean sucesso = leituraSensorDAO.deletarLeitura(id);

        if (!sucesso) {
            return ResponseEntity
                    .status(HttpStatus.INTERNAL_SERVER_ERROR)
                    .body("Não foi possível deletar a leitura.");
        }

        return ResponseEntity.ok("Leitura deletada com sucesso!");
    }
    @DeleteMapping("/leituras")
    public ResponseEntity<String> deletarTodasLeituras() {

        boolean sucesso = leituraSensorDAO.deletarTodasLeituras();

        if (!sucesso) {
            return ResponseEntity
                    .status(HttpStatus.INTERNAL_SERVER_ERROR)
                    .body("Erro ao deletar todas as leituras.");
        }

        return ResponseEntity.ok("Todas as leituras foram deletadas com sucesso!");
    }
}