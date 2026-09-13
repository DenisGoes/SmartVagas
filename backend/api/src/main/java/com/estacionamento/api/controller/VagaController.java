package com.estacionamento.api.controller;

import com.estacionamento.api.dao.VagaDAO;
import com.estacionamento.api.models.Vaga;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/smartvagas")
public class VagaController {

    private final VagaDAO vagaDAO = new VagaDAO();

    @PostMapping("/vagas")
    public String criarVaga(@RequestBody Vaga vaga) {
        vagaDAO.cadastrarVaga(vaga);

        return "Vaga " + vaga.getNumero() + " criada com sucesso!";
    }

    @GetMapping("/vagas")
    public List<Vaga> listarVagas() {
        return vagaDAO.listarVagas();
    }

    @GetMapping("/vagas/{id}")
    public Vaga buscarVagaPorId(@PathVariable int id) {
        return vagaDAO.buscarVagaPorId(id);
    }

    @PutMapping("/vagas/{id}")
    public String atualizarVaga(@PathVariable int id, @RequestBody Vaga vagaAtualizada) {
        Vaga existente = vagaDAO.buscarVagaPorId(id);

        if (existente != null) {
            vagaAtualizada.setId(id);
            vagaDAO.atualizarVaga(vagaAtualizada);

            return "Vaga atualizada com sucesso!";
        }
        return "Erro: Vaga não encontrada.";
    }

    @DeleteMapping("/vagas/{id}")
    public String deletarVaga(@PathVariable int id) {
        Vaga existente = vagaDAO.buscarVagaPorId(id);

        if (existente != null) {
            vagaDAO.deletarVaga(id);

            return "Vaga deletada com sucesso!";
        }

        return "Erro: Vaga não encontrada para deletar.";
    }
}