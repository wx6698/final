pipeline {
     agent any
     stages {
         stage('AWS Credentials') {
             steps {
                withCredentials([[$class: 'AmazonWebServicesCredentialsBinding', credentialsId: 'bg', accessKeyVariable: 'ACCESS_KEY', secretKeyVariable: 'SECRET_KEY']]){
                 sh '''
                     mkdir -p ~/.aws
                     echo "[default]" > ~/.aws/credentials
                     echo "aws_access_key_id = ${ACCESS_KEY}" >> ~/.aws/credentials
                     echo "aws_secret_access_key = ${SECRET_KEY}" >> ~/.aws/credentials
                     echo "[default]" > ~/.boto
                     echo "aws_access_key_id = ${ACCESS_KEY}" >> ~/.boto
                     echo "aws_secret_access_key = ${SECRET_KEY}" >> ~/.boto
                 '''
                }
             }
         }
         stage('Lint HTML') {
              steps {
                  sh '''
                     echo "Validating Dockerfile and app python file..."
                     hadolint Dockerfile
                     pylint --disable=R,C,W1203 app.py
                     # validate Dockerfile and app.py
                     echo "Validating CloudFormation templates..."
                     for template in cloudformation/*.yml; do
                     yamllint --strict $template
                     cfn-lint --include-checks I --template $template
                     # validate-template requires valid AWS Credentials :(
                     # aws cloudformation validate-template --template-body file://$template --region us-east-1
                     done
                     # Check ansible playbooks for correctness.
                     playbooks="main.yml deploy.yml delete.yml"
                     echo "Checking playbook syntax..."
                     ansible-playbook -i inventory $playbooks --syntax-check
                     echo "Linting playbooks with ansible-lint..."
                     ansible-lint $playbooks
                     '''
              }
         }
         stage('Deploy EKS cluster') {
              steps {
                  'ansible-playbook -i inventory main.yml'
              } 
         }
         stage('Deploy Docker to EKS') {
              steps {
                  './deploy.sh'
              } 
         }
     }
}
