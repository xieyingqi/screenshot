import keyboard
import time
from PIL import ImageGrab
from aip import AipOcr

""" 你的 APPID AK SK """
APP_ID = '15537111'
API_KEY = 'sGwVtD61Qr0K3p6iTdL1FgGj'
SECRET_KEY = 'VFjq6GG6bNVBDSbYcb7008iRKds5Bpar'

client = AipOcr(APP_ID, API_KEY, SECRET_KEY)


""" 读取图片 """
def get_file_content(filePath):
    with open(filePath, 'rb') as fp:
        return fp.read()

myfile = open("./temp.txt",'w')
image = get_file_content('screen.jpg')

""" 调用通用文字识别（高精度版） """
text = client.basicAccurate(image)
result = text['words_result']
for i in result:
    #print (i['words'])
    myfile.writelines(i['words']+'\n')