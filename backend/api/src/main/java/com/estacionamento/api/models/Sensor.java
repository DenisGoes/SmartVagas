package com.estacionamento.api.models;

public class Sensor {
    private Integer id;
    private String nome;
    private Integer vagaId;

    public Sensor() {
    }

    public Sensor(String nome, Integer vagaId) {
        this.nome = nome;
        this.vagaId = vagaId;
    }

    public Sensor(Integer id, String nome, Integer vagaId) {
        this.id = id;
        this.nome = nome;
        this.vagaId = vagaId;
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

    public Integer getVagaId() {
        return vagaId;
    }

    public void setVagaId(Integer vagaId) {
        this.vagaId = vagaId;
    }
}