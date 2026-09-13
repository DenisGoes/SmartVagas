package com.estacionamento.api.controller;

import com.estacionamento.api.dao.EstacionamentoDAO;
import com.estacionamento.api.models.Estacionamento;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/smartvagas")
public class EstacionamentoController {

    private final EstacionamentoDAO estacionamentoDAO = new EstacionamentoDAO();

    @PostMapping("/estacionamentos")
    public String criarEstacionamento(@RequestBody Estacionamento estacionamento) {
        estacionamentoDAO.cadastrarEstacionamento(estacionamento);

        return "Estacionamento " + estacionamento.getNome() + " criado com sucesso!";
    }

    @GetMapping("/estacionamentos")
    public List<Estacionamento> listarEstacionamentos() {
        return estacionamentoDAO.listarEstacionamentos();
    }

    @GetMapping("/estacionamentos/{id}")
    public Estacionamento buscarEstacionamentoPorId(@PathVariable int id) {
        return estacionamentoDAO.buscarEstacionamentoPorId(id);
    }

    @PutMapping("/estacionamentos/{id}")
    public String atualizarEstacionamento(@PathVariable int id, @RequestBody Estacionamento estacionamentoAtualizado) {
        Estacionamento existente = estacionamentoDAO.buscarEstacionamentoPorId(id);

        if (existente != null) {
            estacionamentoAtualizado.setId(id);
            estacionamentoDAO.atualizarEstacionamento(estacionamentoAtualizado);

            return "Estacionamento atualizado com sucesso!";
        }
        return "Erro: Estacionamento não encontrado.";
    }

    @DeleteMapping("/estacionamentos/{id}")
    public String deletarEstacionamento(@PathVariable int id) {
        Estacionamento existente = estacionamentoDAO.buscarEstacionamentoPorId(id);

        if (existente != null) {
            estacionamentoDAO.deletarEstacionamento(id);

            return "Estacionamento deletado com sucesso!";
        }
        return "Erro: Estacionamento não encontrado para deletar.";
    }
}

