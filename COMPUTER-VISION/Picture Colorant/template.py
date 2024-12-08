# Feel free to change / extend / adapt this source code as needed to complete the homework, based on its requirements.
# This code is given as a starting point.
#
# REFEFERENCES
# The code is partly adapted from pytorch tutorials, including https://pytorch.org/tutorials/beginner/blitz/cifar10_tutorial.html

# ---- hyper-parameters ----
# You should tune these hyper-parameters using:
# (i) your reasoning and observations, 
# (ii) by tuning it on the validation set, using the techniques discussed in class.
# You definitely can add more hyper-parameters here.
batch_size = 16
max_num_epoch = 100
hps = {'lr':0.01}

# ---- options ----
DEVICE_ID = 'cpu' # set to 'cpu' for cpu, 'cuda' / 'cuda:0' or similar for gpu.
LOG_DIR = 'checkpoints'
VISUALIZE = False # set True to visualize input, prediction and the output from the last batch
LOAD_CHKPT = False

# --- imports ---
import torch
import os
import matplotlib.pyplot as plt
import numpy as np
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
import torchvision.transforms as transforms
import hw3utils
torch.multiprocessing.set_start_method('spawn', force=True)
# ---- utility functions -----
def get_loaders(batch_size,device):
    data_root = 'ceng483-hw3-dataset' 
    train_set = hw3utils.HW3ImageFolder(root=os.path.join(data_root,'train'),device=device)
    train_loader = torch.utils.data.DataLoader(train_set, batch_size=batch_size, shuffle=True, num_workers=0)
    val_set = hw3utils.HW3ImageFolder(root=os.path.join(data_root,'val'),device=device)
    val_loader = torch.utils.data.DataLoader(val_set, batch_size=batch_size, shuffle=False, num_workers=0)
    # Note: you may later add test_loader to here.
    return train_loader, val_loader

# ---- ConvNet -----
class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.conv1 = nn.Conv2d(1, 16, kernel_size=3,padding=1)
        self.bn = nn.BatchNorm2d(8)
        self.conv3 = nn.Conv2d(16,16,kernel_size=3,padding=1)
        self.bn2 = nn.BatchNorm2d(3)
        self.relu = nn.ReLU()
        self.tanh = nn.Tanh()
        self.conv2 = nn.Conv2d(16, 16, kernel_size=3, padding=1)
        self.conv4 = nn.Conv2d(16, 3, kernel_size=3, padding=1)

    def forward(self, grayscale_image):
        # Apply the network's layers
        x = self.relu(self.conv1(grayscale_image))
        x = self.relu(self.conv2(x))
        x = self.relu(self.conv3(x))
        x = self.tanh(self.conv4(x))
        return x

# ---- training code -----
device = torch.device(DEVICE_ID)
print('device: ' + str(device))
net = Net().to(device=device)
criterion = nn.MSELoss()
optimizer = optim.SGD(net.parameters(), lr=hps['lr'])
train_loader, val_loader = get_loaders(batch_size,device)

if LOAD_CHKPT:
    print('loading the model from the checkpoint')
    model.load_state_dict(os.path.join(LOG_DIR,'checkpoint.pt'))

print('training begins')
previous_loss = 0
for epoch in range(max_num_epoch):  
    running_loss = 0.0 # training loss of the network
    current_loss = 0
    for iteri, data in enumerate(train_loader, 0):
        inputs, targets = data # inputs: low-resolution images, targets: high-resolution images.

        optimizer.zero_grad() # zero the parameter gradients

        # do forward, backward, SGD step
        preds = net(inputs)
        loss = criterion(preds, targets)
        loss.backward()
        optimizer.step()

        # print loss
        running_loss += loss.item()
        current_loss = running_loss / 100
        print_n = 100 # feel free to change this constant
        if iteri % print_n == (print_n-1):    # print every print_n mini-batches
            print('[%d, %5d] network-loss: %.3f' %
                  (epoch + 1, iteri + 1, running_loss / 100))
            running_loss = 0.0
            # note: you most probably want to track the progress on the validation set as well (needs to be implemented)

        if (iteri==0) and VISUALIZE: 
            hw3utils.visualize_batch(inputs,preds,targets)

    print('Saving the model, end of epoch %d' % (epoch+1))

    if not os.path.exists(LOG_DIR):
        os.makedirs(LOG_DIR)
    torch.save(net.state_dict(), os.path.join(LOG_DIR,'checkpoint.pt'))
    hw3utils.visualize_batch(inputs,preds,targets,os.path.join(LOG_DIR,'example.png'))
    print(previous_loss, current_loss)
    if abs(previous_loss - current_loss) <= 0.00001:
        break
    previous_loss = current_loss
print('Finished Training')
net.eval()  # Set the network to evaluation mode

with torch.no_grad():  # Disable gradient calculation
    val_loss = 0.0
    # Optionally, other metrics can be calculated, such as accuracy

    for data in val_loader:
        inputs, targets = data
        inputs, targets = inputs.to(device), targets.to(device)

        # Forward pass
        preds = net(inputs)

        # Compute the loss
        loss = criterion(preds, targets)
        val_loss += loss.item()

    avg_val_loss = val_loss / len(val_loader)
    hw3utils.visualize_batch(inputs, preds, targets, os.path.join(LOG_DIR, 'example.png'))
    print('Validation Loss: {:.3f}'.format(avg_val_loss))


