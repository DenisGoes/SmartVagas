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
		initializer.criarTabelaSensor();
		initializer.criarTabelaLeiturasSensor();
}
}
