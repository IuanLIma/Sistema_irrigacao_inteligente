CREATE DATABASE IF NOT EXISTS BDIrrigação;
USE BDIrrigação;

create table if not exists tbSensors(
    id bigint Primary key auto_increment,
    soil decimal(5,2) not null,
    humidity decimal(5,2) not null,
    temperature decimal(5,2) not null,
    created_at timestamp default current_timestamp
);

create table if not exists tbIAdecision(
    id bigint Primary key auto_increment,
    reading_id bigint,
    irrigate tinyint(1) not null default 1,
    accuracy decimal(5,2) not null, -- Alterado para 5,2 para aceitar porcentagens (ex: 95.50)
    created_at timestamp default current_timestamp,
    foreign key(reading_id) references tbSensors(id)
);

create table if not exists tbIrrigationActions(
    id bigint Primary key auto_increment,
    decision_id bigint,
    action tinyint(1) not null default 1,
    executed_at timestamp default current_timestamp,
    foreign key(decision_id) references tbIAdecision(id)
);