import pandas as pd
import numpy as np
from sklearn.ensemble import RandomForestClassifier
import joblib

print("1. Gerando dados alinhados com a estrutura do novo Banco de Dados...")
np.random.seed(42)

# O Arduino e o Banco agora possuem 3 variáveis novamente
temperaturas = np.random.uniform(10.0, 40.0, 3000)
umidades_ar = np.random.uniform(20.0, 90.0, 3000)
umidades_solo = np.random.uniform(10.0, 100.0, 3000)

dados = []
for t, u_ar, u_solo in zip(temperaturas, umidades_ar, umidades_solo):
    # A tabela tbIAdecision do seu amigo foca estritamente na irrigação (Ligado 1 / Desligado 0)
    # Mantivemos a lógica base: Solo abaixo de 50% dispara a irrigação
    irrigar = 1 if u_solo < 50 else 0
    
    dados.append([t, u_ar, u_solo, irrigar])

df = pd.DataFrame(dados, columns=['Temp', 'Umid_Ar', 'Umid_Solo', 'Irrigar'])

print("2. Separando Entradas (3 sensores) e Saída (1 decisão)...")
# A IA agora será alimentada pelas 3 colunas da tbSensors
X = df[['Temp', 'Umid_Ar', 'Umid_Solo']]
y = df['Irrigar'] 

print("3. Treinando o modelo Classificador...")
# Voltamos para o Classifier para que a API consiga usar o cálculo de "accuracy"
modelo_ia = RandomForestClassifier(n_estimators=50, random_state=42)
modelo_ia.fit(X, y)

print("4. Salvando o 'cérebro' da IA...")
joblib.dump(modelo_ia, 'modelo_estufa.pkl')

print("\nSUCESSO! O arquivo 'modelo_estufa.pkl' foi reescrito.")
print("Agora ele mapeia 3 sensores e gera o cálculo de precisão (accuracy) para a API.")