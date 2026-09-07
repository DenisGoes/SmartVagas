package com.estacionamento.api;

import com.estacionamento.api.database.Conexao;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import java.sql.Connection;

@SpringBootApplication
public class ApiApplication {

	public static void main(String[] args) {
		SpringApplication.run(ApiApplication.class, args);

		Connection conexao = Conexao.conectar();
		System.out.println(conexao);
	}

}
