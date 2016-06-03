#
# Projekt do predmetu KRY na vypocet privatniho klice z verejneho klice,
# ktery byl vygenerovan pomoc EC.
# Autor: Vojtech Havlena, xhavle03
# Datum: 11. 3. 2016
#


#
# Rozsireny Eukliduv algoritmus, ktery hleda Bezoutovy koeficienty.
# Je nutne pro vypocet inverzniho prvku v konecnem poli. Orezeno jen
# na nezbytnou cast.
# Vstup: a -- prvek, jehoz inverz se ma spocitat, b -- modulo.
#
def inverse(a, b):
    x = 0 
    y = 1 
    u = 1
    while a != 0:
        q, r = b//a, b%a
        k = x-u*q
        b = a
        a = r
        x = u
        u = k
    return x

#
# Funkce, ktera zapouzdruje operace Point addition a Point doubling
# pro EC nad F_p. 
# Vstup: souradnice bodu P, Q, parametry p, a
#
def computePlusPoint(xP, yP, xQ, yQ, p, a):
	if xP == xQ and yP == -yQ:
		return (0,0)
	elif xP == xQ and yP == yQ:
		f = inverse((2*yP) % p, p)
		lam = f*(((3*xP*xP) % p) + a) % p
		xR = (lam*lam - 2*xP) % p
		yR = (lam*(xP - xR) - yP) % p
		return (xR, yR)
	else:
		f = inverse((xQ - xP) % p, p)
		lam = (((yQ - yP) % p) * f) % p
		xR = (lam*lam - xP - xQ) % p
		yR = (lam*(xP - xR) - yP) % p
		return (xR, yR)

# Konstanty pro hledani privatniho klice.
limit = 1000;
xP = 0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296
yP = 0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5
xQ = 0x52910a011565810be90d03a299cb55851bab33236b7459b21db82b9f5c1874fe
yQ = 0xe3d03339f660528d511c2b1865bcdfd105490ffc4c597233dd2b2504ca42a562
p = 0xffffffff00000001000000000000000000000000ffffffffffffffffffffffff
a = -3

xR = xP
yR = yP

# Postupne hledani klice hrubou silou dP = Q.
for i in range(1,limit):
	if xR == xQ and yR == yQ:
		print i
		break
	xR, yR = computePlusPoint(xR, yR, xP, yP, p, a)
		

