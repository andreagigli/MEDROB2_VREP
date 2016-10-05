#include "robot_utilities.h"



Matrix4f linkCoordTransform(float a, float alpha, float d, float theta)
{
	Matrix4f result(4, 4);

	result(0, 0) = cos(theta);
	result(0, 1) = -sin(theta)*cos(alpha);
	result(0, 2) = sin(theta)*sin(alpha);
	result(0, 3) = a*cos(theta);

	result(1, 0) = sin(theta);
	result(1, 1) = cos(theta)*cos(alpha);
	result(1, 2) = -cos(theta)*sin(alpha);
	result(1, 3) = a*sin(theta);

	result(2, 0) = 0;
	result(2, 1) = sin(alpha);
	result(2, 2) = cos(alpha);
	result(2, 3) = d;

	result(3, 0) = 0;
	result(3, 1) = 0;
	result(3, 2) = 0;
	result(3, 3) = 1;

	return result;
}



//funzione che restituisce la cinematica diretta del manipolatore espressa con una matrice di trasformazione
//omogenea, in funzione del valore delle variabili di giunto, dei parametri cinematici e dei parametri
//della notazione di Denavit-Hartemberg
Matrix4f cinematicaDiretta(VectorXf a_vec, VectorXf alpha_vec, VectorXf d_vec, VectorXf joint_coord)
{
	Matrix4f result;

	//inizializzo la matrice con la matrice identit�
	result.setIdentity();

	int joint_number = joint_coord.size();

	//questo vettore conterr� tutte le matrici di trasformazione di coordinate tra le terne associate a due link consecutivi.
	//Il primo elemento (in posizione 0) si riferisce alla matrice A^0_1, il secondo alla matrice A^1_2, e cos� via
	vector<Matrix4f> transform_matrices;

	for (int i = 0; i<joint_number; i++)
	{ //aggiunge elementi in coda al vettore
		transform_matrices.push_back(linkCoordTransform(a_vec(i), alpha_vec(i), d_vec(i), joint_coord(i)));
	}

	//effettuo il prodotto delle matrici di trasformazione per calcolare la cinematica diretta
	for (int i = 0; i<joint_number; i++)
	{
		Matrix4f temp;
		temp = result * transform_matrices.at(i);
		result = temp;
	}

	return result;
}



//funzione che restituisce la cinematica diretta del manipolatore CALCOLATA FINO AL GIUNTO i incluso (Ai-i,i inclusa),
//espressa con una matrice di trasformazione omogenea, in funzione del valore delle variabili di giunto,
//dei parametri cinematici e dei parametri della notazione di Denavit-Hartemberg
Matrix4f cinematicaDiretta(VectorXf a_vec, VectorXf alpha_vec, VectorXf d_vec, VectorXf joint_coord, int i_pos)
{
	Matrix4f result;

	//inizializzo la matrice con la matrice identit�
	result.setIdentity();

	//questo vettore conterr� tutte le matrici di trasformazione di coordinate tra le terne associate a due link consecutivi.
	//Il primo elemento (in posizione 0) si riferisce alla matrice A^0_1, il secondo alla matrice A^1_2, e cos� via fino a Ai-1,i
	vector<Matrix4f> transform_matrices;

	for (int i = 0; i<i_pos; i++)
	{ //aggiunge elementi in coda al vettore
		transform_matrices.push_back(linkCoordTransform(a_vec(i), alpha_vec(i), d_vec(i), joint_coord(i)));
	}

	//effettuo il prodotto delle matrici di trasformazione per calcolare la cinematica diretta
	for (int i = 0; i<i_pos; i++)
	{
		result = result * transform_matrices.at(i);
	}

	return result;
}


//funzione che restituisce lo Jacobiano geometrico
MatrixXf jacobianoGeometrico(VectorXf a_vec, VectorXf alpha_vec, VectorXf d_vec, VectorXf joint_coord)
{/*lo Jacobiano geometrico per il Kuka LWR � una matrice 6x7 ed assume la seguente espressione:

 | z0 x (p-p0)  z1 x (p-p1)  z2 x (p-p2)  z3 x (p-p3)  z4 x (p-p4)  z5 x (p-p5)  z6 x (p-p6)  |
 J(q)=|    z0           z1           z2           z3           z4           z5           z6        |

 dove
 z0 = [0 0 1]^T
 p sono i primi 3 elementi della quarta colonna della matrice che esprime la cinematica diretta
 pi solo le prime 3 componenti dell'ultima colonna di  A0,1 * A1,2 * ... Ai-1,i
 Aj-1,j � la matrice di trasformazione in coordinate omogenee associata alla coppia di terne j-1, j
 zi = R0,1 * R1,2 * ... Ri-1,i * z0

 */
	Vector3f z0(0, 0, 1);

	//calcolo delle matrici di trasformazione
	Matrix4f A_0_1 = cinematicaDiretta(a_vec, alpha_vec, d_vec, joint_coord, 1);
	Matrix4f A_0_2 = cinematicaDiretta(a_vec, alpha_vec, d_vec, joint_coord, 2);
	Matrix4f A_0_3 = cinematicaDiretta(a_vec, alpha_vec, d_vec, joint_coord, 3);
	Matrix4f A_0_4 = cinematicaDiretta(a_vec, alpha_vec, d_vec, joint_coord, 4);
	Matrix4f A_0_5 = cinematicaDiretta(a_vec, alpha_vec, d_vec, joint_coord, 5);
	Matrix4f A_0_6 = cinematicaDiretta(a_vec, alpha_vec, d_vec, joint_coord, 6);
	Matrix4f A_0_7 = cinematicaDiretta(a_vec, alpha_vec, d_vec, joint_coord, 7);  //cinematica diretta completa

	Vector3f p = A_0_7.block<3, 1>(0, 3);

	Vector3f p_1 = A_0_1.block<3, 1>(0, 3);
	Vector3f p_2 = A_0_2.block<3, 1>(0, 3);
	Vector3f p_3 = A_0_3.block<3, 1>(0, 3);
	Vector3f p_4 = A_0_4.block<3, 1>(0, 3);
	Vector3f p_5 = A_0_5.block<3, 1>(0, 3);
	Vector3f p_6 = A_0_6.block<3, 1>(0, 3);

	Vector3f z_1 = A_0_1.block<3, 3>(0, 0) * z0;
	Vector3f z_2 = A_0_2.block<3, 3>(0, 0) * z0;
	Vector3f z_3 = A_0_3.block<3, 3>(0, 0) * z0;
	Vector3f z_4 = A_0_4.block<3, 3>(0, 0) * z0;
	Vector3f z_5 = A_0_5.block<3, 3>(0, 0) * z0;
	Vector3f z_6 = A_0_6.block<3, 3>(0, 0) * z0;

	MatrixXf result(6, 7);  //lo Jacobiano da restituire

							//blocco Jp1
	result.block<3, 1>(0, 0) = z0.cross(p);
	//blocco Jp2
	result.block<3, 1>(0, 1) = z_1.cross(p - p_1);
	//blocco Jp3
	result.block<3, 1>(0, 2) = z_2.cross(p - p_2);
	//blocco Jp4
	result.block<3, 1>(0, 3) = z_3.cross(p - p_3);
	//blocco Jp5
	result.block<3, 1>(0, 4) = z_4.cross(p - p_4);
	//blocco Jp6
	result.block<3, 1>(0, 5) = z_5.cross(p - p_5);
	//blocco Jp7
	result.block<3, 1>(0, 6) = z_6.cross(p - p_6);

	//blocco Jo1
	result.block<3, 1>(3, 0) = z0;
	//blocco Jo2
	result.block<3, 1>(3, 1) = z_1;
	//blocco Jo3
	result.block<3, 1>(3, 2) = z_2;
	//blocco Jo4
	result.block<3, 1>(3, 3) = z_3;
	//blocco Jo5
	result.block<3, 1>(3, 4) = z_4;
	//blocco Jo6
	result.block<3, 1>(3, 5) = z_5;
	//blocco Jo7
	result.block<3, 1>(3, 6) = z_6;

	return result;
}


//funzione che imposta i parametri di Denavit Hartenberg per il Kuka LWR
void setDHParameter(int DOF_number, VectorXf& a_vec, VectorXf& alpha_vec, VectorXf& d_vec)
{ 	//imposto i parametri cinematici del robot
	float lung_0 = 115.0f;
	float lung_1 = 200.0f;
	float lung_2 = 200.0f;
	float lung_3 = 200.0f;
	float lung_4 = 200.0f;
	float lung_5 = 190.0f;
	float lung_7 = 112.0f;
	float offset = 78.0f;


	a_vec.resize(DOF_number);
	alpha_vec.resize(DOF_number);
	d_vec.resize(DOF_number);

	//inizializzazione dei parametri cinematici e dei parametri della notazione di Denavit-Hartenbger
	a_vec.setZero(DOF_number);

	alpha_vec(0) = (float)M_PI / 2;
	alpha_vec(1) = (float)-M_PI / 2;
	alpha_vec(2) = (float)M_PI / 2;
	alpha_vec(3) = (float)-M_PI / 2;
	alpha_vec(4) = (float)M_PI / 2;
	alpha_vec(5) = (float)-M_PI / 2;
	alpha_vec(6) = (float)0.0f;

	d_vec(0) = lung_0 + lung_1;
	d_vec(1) = 0;
	d_vec(2) = lung_2 + lung_3;
	d_vec(3) = 0;
	d_vec(4) = lung_4 + lung_5;
	d_vec(5) = 0;
	d_vec(6) = lung_7 + offset;

}

MatrixXf LWRGeometricJacobian(std::vector<float>& lwr_joint_q)
{ //strutture dati per i parametri di Denavit Hartenberg
	VectorXf a_vec;
	VectorXf alpha_vec;
	VectorXf d_vec;
	VectorXf joint_coord(7);

	//copio in una struttura VectorXf (7x1) i valori dei giunti della configurazione corrente
	//escludendo i primi 6, che sono associati ad un free flying joint
	for (int i = 0; i < 7; i++)
		joint_coord(i - 6) = lwr_joint_q[i];

	setDHParameter(lwr_joint_q.size(), a_vec, alpha_vec, d_vec);

	return jacobianoGeometrico(a_vec, alpha_vec, d_vec, joint_coord);
}


void computeNullSpaceVelocity(VectorXf& config_q_dot, 
	VectorXf& des_vel, VectorXf& des_pose, VectorXf& curr_pose,
	MatrixXf& J, MatrixXf& Kp)
{
	MatrixXd temp = J * J.transpose();
	MatrixXf pinv_J = J.transpose() * temp.inverse();

	VectorXf auxiliary_vector(7);
	for (int j = 0; j<7; j++)
		auxiliary_vector(j) = (float)rand() / (float)RAND_MAX;

	VectorXf range_space_velocities(7), null_space_velocities(7);
	MatrixXf I(7, 7);
	VectorXf r_dot(6);
	r_dot = des_vel + Kp *(des_pose - curr_pose);

	range_space_velocities = pinv_J * r_dot;
	null_space_velocities = (I.setIdentity() - pinv_J * J) * auxiliary_vector;

	config_q_dot = range_space_velocities + null_space_velocities;
}


void computeDLSVelocity(VectorXf& config_q_dot,
	VectorXf& des_vel, VectorXf& des_pose, VectorXf& curr_pose,
	MatrixXf& J, MatrixXf Kp,float mu)
{
	MatrixXf I(6, 6);
	VectorXf r_dot(6);
	r_dot = des_vel + Kp *(des_pose - curr_pose);

	config_q_dot = J.transpose() * (J * J.transpose() + mu*mu*I).inverse() * r_dot;
}