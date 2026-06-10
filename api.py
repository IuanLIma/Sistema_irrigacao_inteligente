from flask import Flask, request, jsonify
import mysql.connector
import joblib

app = Flask(__name__)

modelo_ia = joblib.load('modelo_estufa.pkl')

DB_CONFIG = {
    'host': 'localhost',
    'port': 3306,
    'user': 'root',
    'password': 'Aventura@50',
    'database': 'BDIrrigação'
}

@app.route('/receber_dados', methods=['POST'])
def receber_dados():
    dados = request.json

    temp = dados.get('temperatura')
    umid_ar = dados.get('umidade_ar')
    umid_solo = dados.get('umidade_solo')

    previsao = modelo_ia.predict([[temp, umid_ar, umid_solo]])
    probabilidades = modelo_ia.predict_proba([[temp, umid_ar, umid_solo]])

    decisao = int(previsao[0])
    certeza = round(probabilidades[0][decisao] * 100, 2)

    conexao = mysql.connector.connect(**DB_CONFIG)
    cursor = conexao.cursor()

    cursor.execute(
        "INSERT INTO tbSensors (soil, humidity, temperature) VALUES (%s, %s, %s)",
        (umid_solo, umid_ar, temp)
    )
    reading_id = cursor.lastrowid

    cursor.execute(
        "INSERT INTO tbIAdecision (reading_id, irrigate, accuracy) VALUES (%s, %s, %s)",
        (reading_id, decisao, certeza)
    )
    decision_id = cursor.lastrowid

    conexao.commit()
    cursor.close()
    conexao.close()

    return jsonify({
        "irrigar": decisao,
        "decision_id": decision_id
    })

@app.route('/confirmar_acao', methods=['POST'])
def confirmar_acao():
    dados = request.json
    decision_id = dados.get('decision_id')
    action = dados.get('action')

    conexao = mysql.connector.connect(**DB_CONFIG)
    cursor = conexao.cursor()

    cursor.execute(
        "INSERT INTO tbIrrigationActions (decision_id, action) VALUES (%s, %s)",
        (decision_id, action)
    )

    conexao.commit()
    cursor.close()
    conexao.close()

    return jsonify({"status": "ok"})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)