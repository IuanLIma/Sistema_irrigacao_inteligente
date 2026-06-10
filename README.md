# 🌱 Sistema de Irrigação Inteligente

Sistema de irrigação automatizada utilizando **ESP32, API REST e Machine Learning** para tomada de decisão baseada em dados ambientais.

## 🚀 Tecnologias

* ESP32 (C/C++)
* Python (Flask)
* MySQL
* Random Forest (Machine Learning)

## ⚙️ Como funciona

O ESP32 coleta:

* Temperatura
* Umidade do ar
* Umidade do solo

Esses dados são enviados para uma API, que utiliza um modelo de **Random Forest** para decidir:

* `1` → Irrigar
* `0` → Não irrigar

A decisão é retornada ao ESP32, que aciona (ou não) a irrigação.

## 🧠 Inteligência Artificial

O modelo analisa:

```text
X = [temperatura, umidade_ar, umidade_solo]
```

Além da decisão, ele retorna a **probabilidade (confiança)**, baseada na votação das árvores do modelo.

## 🔄 Comportamento

* Irrigando → nova leitura em 1 minuto
* Não irrigando → nova leitura em 1 hora

## 📌 Destaques

* Integração completa IoT + Backend + IA
* Tomada de decisão automatizada
* Sistema adaptativo em tempo real

## 👨‍💻 Autor

Iuan de Oliveira Lima
