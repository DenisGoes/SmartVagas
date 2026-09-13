package com.estacionamento.api.models;

public class Estacionamento {
    private Integer id;
    private String nome;

    public Estacionamento(){}

    public Estacionamento(String nome){
        this.nome = nome;
    }

    public Estacionamento(Integer id, String nome){
        this.id = id;
        this.nome = nome;
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getNome() {
        return nome;
    }

    public void setNome(String nome) {
        this.nome = nome;
    }

}