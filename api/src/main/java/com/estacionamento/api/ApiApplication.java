package com.estacionamento.api;


import com.estacionamento.api.dao.EstacionamentoDAO;
import com.estacionamento.api.dao.VagaDAO;
import com.estacionamento.api.database.ConnectionFactory;
import com.estacionamento.api.database.DatabaseInitializer;
import com.estacionamento.api.models.Estacionamento;
import com.estacionamento.api.models.Vaga;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import java.sql.Connection;
import java.util.List;

@SpringBootApplication
public class ApiApplication {

	public static void main(String[] args) {
		SpringApplication.run(ApiApplication.class, args);

		DatabaseInitializer initializer = new DatabaseInitializer();
		initializer.criarTabelaEstacionamento();
		initializer.criarTabelaVaga();


		Estacionamento estacionamento = new Estacionamento();
		estacionamento.setNome("SENAI VILA LEOPOLDINA");

		EstacionamentoDAO estacionamentoDAO = new EstacionamentoDAO();
		estacionamentoDAO.cadastrarEstacionamento(estacionamento);

		Vaga vaga = new Vaga();
		vaga.setNumero(5);
		vaga.setStatus("LIVRE");
		vaga.setEstacionamento_id(1);

		VagaDAO vagaDAO = new VagaDAO();
		vagaDAO.cadastrarVaga(vaga);


		List<Estacionamento> estacionamentoList = estacionamentoDAO.listarEstacionamentos();

		for(Estacionamento e : estacionamentoList){
			System.out.println("Estacionamentos:\n " + e.getId() + " - " + e.getNome());
		}

		List<Vaga> vagaList = vagaDAO.listarVagas();

		for(Vaga e : vagaList){
			System.out.println("Vagas: \n" + e.getId() + " - " + e.getNumero() + " - " + e.getStatus() + " - " + e.getEstacionamento_id());
		}

}
}
