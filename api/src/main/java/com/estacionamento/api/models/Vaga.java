package com.estacionamento.api.models;

public class Vaga {
    private Integer id;
    private Integer numero;
    private String status;
    private Integer estacionamento_id;

    public Vaga(){}

    public Vaga(Integer numero, String status, Integer estacionamento_id){
        this.numero = numero;
        this.status = status;
        this.estacionamento_id = estacionamento_id;
    }

    public Vaga(Integer id, Integer numero, String status, Integer estacionamento_id){
        this.id = id;
        this.numero = numero;
        this.status = status;
        this.estacionamento_id = estacionamento_id;
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public Integer getNumero() {
        return numero;
    }

    public void setNumero(Integer numero) {
        this.numero = numero;
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public Integer getEstacionamento_id() {
        return estacionamento_id;
    }

    public void setEstacionamento_id(Integer estacionamento_id) {
        this.estacionamento_id = estacionamento_id;
    }
}
