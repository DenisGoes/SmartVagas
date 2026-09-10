package com.estacionamento.api.dao;

import com.estacionamento.api.database.ConnectionFactory;
import com.estacionamento.api.models.Vaga;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;

public class VagaDAO {

    public void cadastrarVaga(Vaga vaga){
        String sql = """
                INSERT INTO vaga(numero, status, estacionamento_id)
                VALUES(?, ?, ?)
                """;

        try(Connection connection = ConnectionFactory.getConnection();
            PreparedStatement stmt = connection.prepareStatement(sql)){

            stmt.setInt(1, vaga.getNumero());
            stmt.setString(2, vaga.getStatus());
            stmt.setInt(3, vaga.getEstacionamento_id());

            stmt.executeUpdate();
            System.out.println("Vaga criada com sucesso!");
        }catch (SQLException e){
            System.out.println("Erro ao criar vaga: " + e.getMessage());
        }
    }

    public List<Vaga> listarVagas(){
        List<Vaga> vagaList = new ArrayList<>();
        String sql = "SELECT * FROM vaga";

        try (Connection connection = ConnectionFactory.getConnection();
        PreparedStatement stmt = connection.prepareStatement(sql);
        ResultSet rs = stmt.executeQuery()){

            while (rs.next()){
                Vaga vaga = new Vaga();

                vaga.setId(rs.getInt("id"));
                vaga.setNumero(rs.getInt("numero"));
                vaga.setStatus(rs.getString("status"));
                vaga.setEstacionamento_id(rs.getInt("estacionamento_id"));

                vagaList.add(vaga);
            }

        }catch (SQLException e){
            System.out.println("Erro ao listar as vagas: " + e.getMessage());
        }
        return vagaList;
    }

    public void deletarVaga(int id){
        String sql = "DELETE FROM vaga WHERE ID = ?";

        try(Connection connection = ConnectionFactory.getConnection();
        PreparedStatement stmt = connection.prepareStatement(sql)){

            stmt.setInt(1, id);

            int linhas = stmt.executeUpdate();
            if(linhas > 0){
                System.out.println("Vaga deletada com sucesso!");
            }else{
                System.out.println("Nenhuma vaga encontrada com o ID informado.");
            }

        }catch (SQLException e){
            System.out.println("Erro ao deletar a vaga: " + e.getMessage());
        }


    }

}
