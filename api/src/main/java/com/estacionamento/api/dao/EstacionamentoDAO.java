package com.estacionamento.api.dao;

import com.estacionamento.api.database.ConnectionFactory;
import com.estacionamento.api.models.Estacionamento;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;

public class EstacionamentoDAO {

    public void cadastrarEstacionamento(Estacionamento estacionamento){
        String sql = "INSERT INTO estacionamento (nome) VALUES (?)";

        try(Connection connection = ConnectionFactory.getConnection();
            PreparedStatement stmt = connection.prepareStatement(sql)){

            stmt.setString(1, estacionamento.getNome());
            stmt.executeUpdate();
            System.out.println("Estacionamento cadastrado com sucesso!");

        }catch (SQLException e){
            System.out.println("Erro ao cadastrar estacionamento." + e.getMessage());
        }
    }

    public List<Estacionamento> listarEstacionamentos(){
        List<Estacionamento> estacionamentoList = new ArrayList<>();
        String sql = "SELECT * FROM estacionamento";

        try(Connection connection = ConnectionFactory.getConnection();
        PreparedStatement stmt = connection.prepareStatement(sql);
        ResultSet rs = stmt.executeQuery()){

            while (rs.next()){
                Estacionamento estacionamento = new Estacionamento();

                estacionamento.setId(rs.getInt("id"));
                estacionamento.setNome(rs.getString("nome"));

                estacionamentoList.add(estacionamento);
            }
        }catch (SQLException e){
            System.out.println("Erro ao listar os estacionamentos!" + e.getMessage());
        }
        return estacionamentoList;
    }

    public void deletarEstacionamento(int id){
        String sql = "DELETE FROM estacionamento WHERE id = ?";

        try (Connection connection = ConnectionFactory.getConnection();
        PreparedStatement stmt = connection.prepareStatement(sql)){

            stmt.setInt(1, id);
            stmt.executeUpdate();
            System.out.println("Estacionamento deletado com sucesso!");
        }catch (SQLException e){
            System.out.println("Erro ao deletar estacionamento!" + e.getMessage());
        }
    }





}
