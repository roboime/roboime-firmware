class PID:

    def __init__(self, P=1.0, I=0.00001, D=0.5, Derivator=0, Integrator=0, Integrator_max=1, Integrator_min=-1, Speed_max=2):

        self.Kp=P          #Ganho Proporcional, aumenta agilidade, e overshooting
        self.Ki=I          #Ganho Integral, relativo aos erros anteriores. É interessante que tenha um valor pequeno para ajuste fino de erros muito pequenos
        self.Kd=D          #Ganho Derivativo, diminui agilidade e overshooting, funciona como um amortecedor quando o robô se aproxima do objetivo
        self.Derivator=Derivator #Armazena erro anterior
        self.Integrator=Integrator #Armazena soma de todos os erros desde o úlitmo ponto de destino setado
        self.Integrator_max=Integrator_max #Valor máximo da integral
        self.Integrator_min=Integrator_min 
        self.Speed_max=Speed_max #velocidade máxima
        self.set_point=0.0 #Ponto de destino
        self.error=0.0 #Erro

    def update(self,current_value): #Recebe posição atual do robo pela câmera e retorna uma velocidade a ser enviada para o robo, deve ser chamada a cada leitura da câmera
 
        self.error = self.set_point - current_value #calcula erro

        self.P_value = self.Kp * self.error #ganho proporcional
        self.D_value = self.Kd * ( self.error - self.Derivator) #ganho diferencial
        self.Derivator = self.error 

        self.Integrator = self.Integrator + self.error #calcula integral

        if self.Integrator > self.Integrator_max: #limita integral
            self.Integrator = self.Integrator_max
        elif self.Integrator < self.Integrator_min:
            self.Integrator = self.Integrator_min

        self.I_value = self.Integrator * self.Ki #ganho integral

        speed = self.P_value + self.I_value + self.D_value #Soma dos ganhos
        
        if abs(speed) > self.Speed_max:   #limita velocidade máxima
            speed = max(speed,-self.Speed_max)
            speed = min(speed, self.Speed_max)
                 
        return speed

    def setPoint(self,set_point): #deve ser chamada a cada vez que mudar o ponto alvo do robo

        self.set_point = set_point
        self.Integrator=0   
        self.Derivator=0

